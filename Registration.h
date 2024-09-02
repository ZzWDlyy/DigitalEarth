#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <string>
#include <opencv2/opencv.hpp>

struct Coordinate {
    double latitude;
    double longitude;
};

struct PixelCoordinate {
    int x;
    int y;
};

struct Image {
    int width;
    int height;
    Coordinate topLeft;
    Coordinate topRight;
    Coordinate bottomLeft;
    Coordinate bottomRight;
};
struct ImageCoordinate {
    Coordinate result[4];
};
class Registration {
public:
    Registration() {};  // 默认构造函数
    void readBigMap(std::string filepath); //读取大图
    cv::Point2f  readMap(std::string filename); //读取一张小图
    double getLatitude();//获取纬度
    double getLongtitude();//获取经度
    ImageCoordinate registerImages(std::string filename, int x, int y); //开始配准
    PixelCoordinate convertToPixelCoordinate();
    Coordinate GetGeoCoordinatesFromPixel(const char* imagePath, int pixelX, int pixelY);
private:
    double UpLeft_Xgeo;
    double	UpLeft_Ygeo;
    double DownRight_Xgeo;
    double	DownRight_Ygeo;
    double lat, lon;
    int bigMapWidth;
    int bigMapheight;
    cv::Mat resizedImage;
    cv::Mat global_image;
};

#endif  // REGISTRATION_H