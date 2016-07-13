#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "adcensus.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    ADCensus *ad_c = new ADCensus();
    engine.rootContext()->setContextProperty("sharedADCensus", ad_c);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
