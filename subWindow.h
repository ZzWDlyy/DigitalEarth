#pragma once

#include <QMainWindow>
#include "ui_subWindow.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QTimer>
#include <QDebug>
class subWindow : public QMainWindow
{
public:
	subWindow(QWidget *parent = nullptr);
	~subWindow();
	void setPath(QString a, QString b, QString c, QString d);
	void uploadImageArg(QString path);
	void replyFinished();
	void startnetRequest();
	void replymatchFinished();
	void startmatchRequest();
	void startmatchRequestone(QString vispath, int k);
	void replymatchFinishedone();
	QString pathIr;
	QString pathGround;
	QString pathMs1;
	QString pathMs2;
	Ui::subWindowClass ui;
private:
	void upload(const QString& url, const QString& filepath);
	QNetworkAccessManager* manager;
	

	QUrl url;
	QNetworkRequest req;
	QNetworkReply* replymatch;
	QNetworkReply* replyfuse;
	QTimer* timer;
	int counter = 0;
	
private slots:
	void uploadImage();
	void uploadImage_2();
	void uploadImage_3();
	void uploadImage_4();
	void updateProgress();
	
};
