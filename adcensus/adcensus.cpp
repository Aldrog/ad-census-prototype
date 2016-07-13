#include "adcensus.h"
#include <QDebug>

const double lambdaCT = 1.0;
const double lambdaAD = 3.0;

ADCensus::ADCensus(QObject *parent) : QObject(parent)
{
}

void ADCensus::constructDisparityMap(QUrl leftImageUrl, QUrl rightImageUrl) {
    QImage leftImage(leftImageUrl.toLocalFile());
    QImage rightImage(rightImageUrl.toLocalFile());
    int width = leftImage.width();
    int height = leftImage.height();
    QImage result(width, height, QImage::Format_Grayscale8);
    // Computations
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y) {
            double minCost = -1;
            int bestDisparity = 0;
            for (int d = 0; d < width - x; ++d) {
                auto c_ad = costAD(leftImage, rightImage, x, y, d);
                auto c_census = costCensus(leftImage, rightImage, x, y, d);
                auto c_full = robust(c_census, lambdaCT) + robust(c_ad, lambdaAD);
                if(minCost < 0 || c_full < minCost) {
                    minCost = c_full;
                    bestDisparity = d;
                }
            }
            qDebug() << x << y;
            qDebug() << bestDisparity;
            result.setPixelColor(x, y, QColor(bestDisparity / width * 255, bestDisparity / width * 255, bestDisparity / width * 255));
        }
    result.save("../../result.png");
    qDebug() << "finished";
}

double ADCensus::costAD(QImage leftImage, QImage rightImage, int x, int y, int disparity) {
    return 0;
}

double ADCensus::costCensus(QImage leftImage, QImage rightImage, int x, int y, int disparity) {
    return 0;
}

double ADCensus::robust(double cost, double lambda) {
    return (1 - exp(-cost / lambda));
}
