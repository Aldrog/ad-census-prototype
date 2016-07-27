#include <QGuiApplication>
#include <QtWidgets/QApplication>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "adcensus.h"

int main(int argc, char *argv[])
{
    //QGuiApplication app(argc, argv);
    QApplication app(argc, argv);

    ADCensus *ad_c = new ADCensus();

    QCommandLineParser parser;
    parser.setApplicationDescription("Stereo vision program on AD-Census-based algorithm");
    parser.addHelpOption();
    QCommandLineOption grayscaleOption("grayscale", "Open images as grayscale");
    parser.addOption(grayscaleOption);
    parser.addPositionalArgument("[URLs]", "Paths to two image files");

    parser.process(app);
    QStringList args = parser.positionalArguments();
    bool gray = parser.isSet(grayscaleOption);

    if(args.length() == 2) {
        if(gray)
            ad_c->disparityMapFromGrayscale(QUrl::fromLocalFile(args[0]),
                                            QUrl::fromLocalFile(args[1]));
        else
            ad_c->disparityMapFromRGB(QUrl::fromLocalFile(args[0]),
                                      QUrl::fromLocalFile(args[1]));
        return 0;
    }

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("sharedADCensus", ad_c);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
