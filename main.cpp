#include "earth_map_demo.h"
#include <QApplication>
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    earth_map_demo w;
    w.show();
    return a.exec();
}
