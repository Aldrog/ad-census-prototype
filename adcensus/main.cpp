#include <QGuiApplication>
#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "adcensus.h"

int main(int argc, char *argv[])
{
    //QGuiApplication app(argc, argv);
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    ADCensus *ad_c = new ADCensus();
    //ad_c->constructDisparityMap(QUrl("file:///home/alexander/work/adcensus/tsukuba0.bmp"),
    //                            QUrl("file:///home/alexander/work/adcensus/tsukuba1.bmp"));
    engine.rootContext()->setContextProperty("sharedADCensus", ad_c);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
