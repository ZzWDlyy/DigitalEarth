#pragma once
#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include "ui_earth_map_demo.h"
#include <osgQOpenGL/osgQOpenGLWidget>
#include <osgDB/ReadFile>
#include <iostream>
#include <string>
#include <osgEarth/EarthManipulator>
#include <osgEarth/ExampleResources>
#include <osgEarth/MapNode>
#include <osgEarth/GDAL>
#include <osgEarth/ImageLayer>
#include <vector>
#include "Location.h"
#include <osgEarth/FeatureImageLayer>
#include <QStandardItemModel>
#include "Registration.h"
#include <opencv2/opencv.hpp>
#include <QWheelEvent>
#include <osgEarth/AnnotationLayer>
#include <QThread>
#include <QDebug>
#include <osgEarth/FeatureNode>
#include <osgEarth/PlaceNode>
#include <osgEarth/ImageOverlay>
#include <QDialog>
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#incude <QtNetwork/QNetworkRequest>
#include <sqlite3.h>
#include <QLabel>
//自定义对话框
class ImageDialog : public QDialog {
public:
    ImageDialog(const QString& imagePath, const QString& title, QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle(title);

        // 创建标签
        QLabel* label = new QLabel(this);
        label->setPixmap(QPixmap(imagePath).scaled(1200, 1200, Qt::KeepAspectRatio));
        label->setAlignment(Qt::AlignCenter);

        // 设置对话框的布局
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(label);
        setLayout(layout);
    }
};

//多线程
class CalculationThread : public QThread {
Q_OBJECT
public:
    explicit CalculationThread(double X, double Y,std::vector<cv::Point2f>& locations, QObject* parent = nullptr) :
        QThread(parent), center(locations),x(X),y(Y){};
    void set(double x1, double y1) {
        x = x1;
        y = y1;
    }
signals:
    void calculationResult(int bestIndex);
protected:
    void run() override {
        while (true) {
            if (final_stop)break;
            if (m_stopRequested) {
                // 在停止请求后重新启动线程
                /*m_stopRequested = false;*/
                continue;
            }
            int bestIndex = calculateBestPoint();
            emit calculationResult(bestIndex);
            QThread::msleep(1300);  // 每次计算后休眠 2 秒
        }
    }
public slots:
    void stop() {
        m_stopRequested = true;
    }
    void restart() {
        m_stopRequested = false;
    }
    void finalstop() {
        final_stop = true;
    }

private:
    bool m_stopRequested = false;
    bool final_stop = false;
    
    std::vector<cv::Point2f> center;
    double x, y;
    int calculateBestPoint() {
        int bestIndex = -1;
        double minDistance = std::numeric_limits<double>::max();

        for (int i = 0; i < center.size(); i++) {
            double latDiff = center[i].y - y;
            double lonDiff = center[i].x - x;
            double distance = std::sqrt(latDiff * latDiff + lonDiff * lonDiff);

            if (distance < minDistance) {
                minDistance = distance;
                bestIndex = i;
            }
        }
        return bestIndex;
    }
};
// 自定义位置数据结构
struct PLocation {
    std::string iscustom;
    std::string locationName;
    std::string latitude;
    std::string longitude;
};
//多线程


class earth_map_demo : public QMainWindow
{
    Q_OBJECT

public:
    earth_map_demo(QWidget *parent = nullptr);
    void region(const QPoint& currentGlobalPoint);  //鼠标的位置,改变光标
    void createAddPositionDialog();
    void addFirstDialog(std::vector<Country*> countries, QComboBox& a, QComboBox& b, QComboBox& c, QComboBox& d);
    void onViewChanged_2(std::string custom, std::string name, double lat1, double log1);
    ~earth_map_demo();
    void startRegistration();
    void startRequest(const QUrl& requestedUrl);
    QString generateUniqueFileName(const QString& filePath);

private:
    bool is_Stop = false;
    bool is_max = false;
    bool is_comboxFinished = false;
    sqlite3* db;//数据库
    osgEarth::AnnotationLayer* annoLayer;
    CalculationThread* calculationThread=NULL;
    int currentIndex = 0;
    int lastIndex = -1;
    osg::ref_ptr<osg::Group>annoGroup;
    std::string filepath;//当前配准的地图的路径
    std::vector<cv::Point2f> center; 
    //是否被替换
    std::vector<bool> is_replace;
    std::vector<ImageCoordinate> pts;
    std::vector<osgEarth::FeatureNode*> featurenode;
    std::vector<osgEarth::PlaceNode*> placenode;
    std::vector<osgEarth::ImageOverlay*>overlaynode;
    Registration *p = nullptr;
    bool featureValid = false;
    bool _isleftpressed = false; //判断是否是左键点击
    int _curpos = 0;    //鼠标左键按下时光标所在区域
    QPoint _plast;      //获取鼠标左键按下时光标在全局(屏幕而非窗口)的位置
    int countRow(QPoint p);            //获取光标在窗口所在区域的 行   返回行数
    int countFlag(QPoint p, int row);    //获取光标在窗口所在区域的 列  返回行列坐标
    void setCursorType(int flag);          //根据传入的坐标，设置光标样式
    bool isCollapsed = false;
    std::vector<PLocation> locationData;  // 存储所有位置对象的容器
    QStandardItemModel* itemModel = nullptr;  // 列表视图的模型
    Ui::earth_map_demoClass ui;
    osg::ref_ptr<osgViewer::Viewer> viewer;
    osg::ref_ptr<osgEarth::MapNode> mapNode;
    osg::ref_ptr<osg::Group>mRoot;
    osg::ref_ptr<osg::Group>groupNode;
    osg::ref_ptr<osg::Group>earthLabel;
    osgQOpenGLWidget* widget;
    osg::ref_ptr<osgEarth::Map> map;
    std::vector<Country*>  countries;
    osg::ref_ptr<osgEarth::Util::EarthManipulator> earthManipulator;
    osgEarth::Util::Controls::LabelControl* bottomStateLabel;
    osgEarth::FeatureImageLayer * featurelayer;
    QStringList fileNames;
    void addViewePointLabel();
    void createMapNode();
    void addMapLayer();
    void XYZtoLHconvert();
    void addCountryline();
    void addCombox();
    void addFirst(std::vector<Country*> countries);
    void addLabel();
    void remove_layer();
    void initialList();
    void displayNextImage();
    void replaceFuseImage();
    void addAllMapLayer(QString path);
    //subWindow* subW;
    //QWidget* subW;
    subWindow* subW;
    QTimer* timer;
    int counter = 0;
protected:
    bool isMax = false;
    bool isCollapse = false;
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void updateProgress();
private slots:
    void onInitialized();
    void onProComboBoxIndexChanged(int);
    void onCityComboBoxIndexChanged(int);
    void onCountryComboBoxIndexChanged(int);
    //void ontownComboBoxIndexChanged(int);
    void onViewChanged();
    void ontbnmaxclicked();
    void onCollapseButtonClicked();
    void onchangeTileMapButtonClicked();
    void handleCalculationResult(int bestIndex);
    void closeW();
    void replaceImage();
    void showImage1();
    void showImage2();
    void showImage3();
    void stopThead();
    void toHome();
    void showsubWidget();
};


