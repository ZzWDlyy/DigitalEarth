#include "Registration.h"
#include <opencv2/opencv.hpp>
#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <ogr_geometry.h>
#include <exiv2/exiv2.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <String>
typedef struct
{
	cv::Point2f left_top;
	cv::Point2f left_bottom;
	cv::Point2f right_top;
	cv::Point2f right_bottom;
}four_corners_t;

four_corners_t corners;


Coordinate Registration::GetGeoCoordinatesFromPixel(const char* imagePath, int pixelX, int pixelY)
{
	Coordinate coordinate;
	coordinate.latitude = 0;
	coordinate.longitude = 0;
	GDALAllRegister();

	GDALDataset* dataset = (GDALDataset*)GDALOpen(imagePath, GA_ReadOnly);
	if (dataset == NULL)
	{
		printf("Failed to open the GeoTIFF file.\n");
		return coordinate;
	}
	double invGeoTransform[6];
	//double geoTransform[6];
	dataset->GetGeoTransform(invGeoTransform);
	//GDALInvGeoTransform(geoTransform, invGeoTransform);
	double geoX = invGeoTransform[0] + pixelX * invGeoTransform[1] + pixelY * invGeoTransform[2];
	double geoY = invGeoTransform[3] + pixelX * invGeoTransform[4] + pixelY * invGeoTransform[5];

	OGRSpatialReference oSRS;
	oSRS.importFromWkt(dataset->GetProjectionRef());
	OGRSpatialReference oLatLong;
	oLatLong.SetWellKnownGeogCS("WGS84");

	OGRCoordinateTransformation* transformation = OGRCreateCoordinateTransformation(&oSRS, &oLatLong);
	if (transformation == NULL)
	{
		printf("Failed to create coordinate transformation.\n");
		GDALClose(dataset);
		return coordinate;
	}

	if (!transformation->Transform(1, &geoX, &geoY))
	{
		printf("Failed to transform coordinates.\n");
		OGRCoordinateTransformation::DestroyCT(transformation);
		GDALClose(dataset);
		return coordinate;
	}

	coordinate.longitude = geoX;
	coordinate.latitude = geoY;

	OGRCoordinateTransformation::DestroyCT(transformation);
	GDALClose(dataset);

	return coordinate;
}

std::string FindExifKey(Exiv2::ExifData& ed, std::string key)
{
	Exiv2::ExifKey tmp = Exiv2::ExifKey(key);
	Exiv2::ExifData::iterator pos = ed.findKey(tmp);
	if (pos == ed.end())
	{
		return "Unknow";
	}
	return pos->value().toString();
}
double ConvertToDecimal(const std::string& gpsString)
{
	std::istringstream iss(gpsString);
	std::string degreesNumeratorString, degreesDenominatorString;
	std::getline(iss, degreesNumeratorString, '/');
	std::getline(iss, degreesDenominatorString, ' ');

	std::string minutesNumeratorString, minutesDenominatorString;
	std::getline(iss, minutesNumeratorString, '/');
	std::getline(iss, minutesDenominatorString, ' ');

	std::string numeratorString, denominatorString;
	std::getline(iss, numeratorString, '/');
	std::getline(iss, denominatorString);

	double degreesNumerator = std::stod(degreesNumeratorString);
	double degreesDenominator = std::stod(degreesDenominatorString);
	double minutesNumerator = std::stod(minutesNumeratorString);
	double minutesDenominator = std::stod(minutesDenominatorString);
	double numerator = std::stod(numeratorString);
	double denominator = std::stod(denominatorString);

	double decimal =
		degreesNumerator + (minutesNumerator / minutesDenominator / 60.0) + (numerator / denominator / 3600.0);
	return decimal;
}



void Registration::readBigMap(std::string filepath) {
	global_image = cv::imread(filepath, cv::ImreadModes::IMREAD_COLOR);
	GDALAllRegister();  //注册所有的驱动
	//CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//以防中文名不能正常读取,不能用干扰了替换
	int num_iamge_size = 0;
	const char* filepath1 = filepath.data();
	GDALDataset* poDataset = (GDALDataset*)GDALOpen(filepath1, GA_ReadOnly);//GDAL数据集
	if (poDataset == NULL)
	{
		std::cout << "指定的文件不能打开!" << std::endl;
		return;
	}
	//获取图像的尺寸
	int nImgSizeX = poDataset->GetRasterXSize();
	int nImgSizeY = poDataset->GetRasterYSize();
	bigMapheight = nImgSizeY;
	bigMapWidth = nImgSizeX;
	std::cout << "ImageX = " << nImgSizeX << ",	ImageY = " << nImgSizeY << std::endl;

	//获取坐标变换系数
	double trans[6];
	CPLErr aaa = poDataset->GetGeoTransform(trans);
	nImgSizeX -= 1;
	nImgSizeY -= 1;
	UpLeft_Xgeo = trans[0];
	UpLeft_Ygeo = trans[3];
	DownRight_Xgeo = trans[0] + nImgSizeX * trans[1] + nImgSizeY * trans[2];
	DownRight_Ygeo = trans[3] + nImgSizeX * trans[4] + nImgSizeY * trans[5];
	GDALClose(poDataset);
}




cv::Point2f Registration::readMap(std::string filename) {
	// 读取图像
	cv::Mat image = cv::imread(filename);
	if (image.empty()) {
		std::cout << "无法读取图像文件" << std::endl;
		return cv::Point2f(0, 0);
	}
	// 调整图像大小
	//cv::Size newSize(image.cols / 10, image.rows / 10);  // 新的图像尺寸为原始尺寸的1/10;
	int k = 10;
	if (image.cols > 3000)
		k = 8;
	else
		k = 8;
	cv::Size newSize(image.cols / k, image.rows / k);
	cv::resize(image, resizedImage, newSize, 0, 0, cv::InterpolationFlags::INTER_LINEAR);
	std::unique_ptr<Exiv2::Image> imageinfo = Exiv2::ImageFactory::open(filename);

	if (imageinfo.get() == 0)
	{
		std::cout << "Read Exif Error." << std::endl;
		return  cv::Point2f(0, 0);
	}
	//读取照片的exif信息  
	imageinfo->readMetadata();
	Exiv2::ExifData ed = imageinfo->exifData();//得到exif信息  

	if (ed.empty())
	{
		std::cout << "Not Find ExifInfo" << std::endl;
		return  cv::Point2f(0, 0);
	}
	lon = ConvertToDecimal(FindExifKey(ed, "Exif.GPSInfo.GPSLongitude"));
	lat = ConvertToDecimal(FindExifKey(ed, "Exif.GPSInfo.GPSLatitude"));
	
	return cv::Point2f(lon, lat);
}

PixelCoordinate Registration::convertToPixelCoordinate() {
	double longitudeDistance = DownRight_Xgeo - UpLeft_Xgeo;
	double latitudeDistance = UpLeft_Ygeo - DownRight_Ygeo;

	double longitudePerPixel = longitudeDistance / bigMapWidth;
	double latitudePerPixel = latitudeDistance / bigMapheight;

	double relativeLongitude = lon - UpLeft_Xgeo;
	double relativeLatitude = UpLeft_Ygeo - lat;

	int x = static_cast<int>(relativeLongitude / longitudePerPixel);
	int y = static_cast<int>(relativeLatitude / latitudePerPixel);

	x = std::max(0, std::min(x, bigMapWidth - 1));
	y = std::max(0, std::min(y, bigMapheight - 1));
	return { x, y };
}

ImageCoordinate Registration::registerImages(std::string filename, int x, int y) {
	// 计算截取大图的区域
	cv::Rect roi(x - 0.3 * resizedImage.cols, y - 0.3 * resizedImage.rows, resizedImage.cols*0.6  , resizedImage.rows*0.6 );
	// 修正截取范围不超出图像边界
	if (roi.x < 0)
		roi.x = 0;
	if (roi.y < 0)
		roi.y = 0;
	if (roi.x + roi.width > global_image.cols)
		roi.width = global_image.cols - roi.x;
	if (roi.y + roi.height > global_image.rows)
		roi.height = global_image.rows - roi.y;
	// 截取大图的区域
	cv::Mat croppedLargeImage = global_image(roi);
	std::string outputFilename = "F:/source/repos/earth_map_demo/earth_map_demo/data/test" + std::to_string(x)+".png";
	cv::Mat img1 = resizedImage;
	cv::Mat img2 = croppedLargeImage;
	cv::Mat img_gray1, img_gray2;
	cv::cvtColor(img1, img_gray1, cv::COLOR_BGR2GRAY);
	cv::cvtColor(img2, img_gray2, cv::COLOR_BGR2GRAY);
	cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
	std::vector<cv::KeyPoint> kp1, kp2;
	cv::Mat des1, des2;
	sift->detectAndCompute(img_gray1, cv::noArray(), kp1, des1);
	sift->detectAndCompute(img_gray2, cv::noArray(), kp2, des2);
	//cv::FlannBasedMatcher matcher;
	cv::BFMatcher matcher;
	std::vector<std::vector<cv::DMatch>> knnMatches;
	matcher.knnMatch(des1, des2, knnMatches, 2);
	std::vector<cv::DMatch> goods;
	for (size_t i = 0; i < knnMatches.size(); ++i)
	{
		if (knnMatches[i][0].distance < 0.7 * knnMatches[i][1].distance)
		{
			goods.push_back(knnMatches[i][0]);
		}
	}

	std::cout << "goods: " << goods.size() << std::endl;

	if (goods.size() >= 4)
	{
		std::vector<cv::Point2f> src_points;
		std::vector<cv::Point2f> des_points;
		for (size_t i = 0; i < goods.size(); ++i)
		{
			src_points.push_back(kp1[goods[i].queryIdx].pt);
			des_points.push_back(kp2[goods[i].trainIdx].pt);
		}

		cv::Mat H = cv::findHomography(src_points, des_points, cv::RANSAC, 5);
		//逆时针
		std::vector<cv::Point2f> pts = { {0.0f, 0.0f}, {0.0f, static_cast<float>(img1.rows - 1)},
								{static_cast<float>(img1.cols - 1), static_cast<float>(img1.rows - 1)},
								{static_cast<float>(img1.cols - 1), 0.0f} };
		std::vector<cv::Point2f> dst;
		cv::perspectiveTransform(pts, dst, H);
		std::vector<cv::Point> dst_int;
		ImageCoordinate coordinate;
		//求出相对于原图中的位置
		for (size_t i = 0; i < dst.size(); ++i)
		{
			dst_int.push_back(cv::Point(dst[i].x + x - 0.3 * resizedImage.cols, dst[i].y + y - 0.3 * resizedImage.rows));
			Coordinate latAndlon = GetGeoCoordinatesFromPixel(filename.data(), dst_int[i].x, dst_int[i].y);
			coordinate.result[i].latitude = latAndlon.latitude;
			coordinate.result[i].longitude = latAndlon.longitude;
		}

		return coordinate;
	}
	else {
		throw std::exception("Not enough matching points found for registration.");
	}

}