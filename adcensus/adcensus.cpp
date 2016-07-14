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
    for (int x = 4; x < width - 4; ++x)
        for (int y = 3; y < height - 3; ++y) {
            double minCost = -1;
            int bestDisparity = 0;
            for (int d = 0; d < x - 4 && d < width / 2; ++d) {
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
    return 1 / 3 * (
            abs(QColor(leftImage.pixel(x, y)).red() - QColor(rightImage.pixel(x - disparity, y)).red()) +
            abs(QColor(leftImage.pixel(x, y)).green() - QColor(rightImage.pixel(x - disparity, y)).green()) +
            abs(QColor(leftImage.pixel(x, y)).blue() - QColor(rightImage.pixel(x - disparity, y)).blue()));
}

double ADCensus::costCensus(QImage leftImage, QImage rightImage, int x, int y, int disparity) {
    int64_t leftCT = 0;
    int64_t rightCT = 0;
    int leftCenter = qGray(leftImage.pixel(x, y));
    int rightCenter = qGray(rightImage.pixel(x - disparity, y));

    for (int i = -4; i < 4; ++i) {
        for (int j = -3; j < 3; ++j) {
            if(i != 0 || j != 0) {
                leftCT += (qGray(leftImage.pixel(x + i, y + j)) >= leftCenter ? 1 : 0);
                rightCT += (qGray(rightImage.pixel(x - disparity + i, y + j)) >= rightCenter ? 1 : 0);
                leftCT = leftCT << 1;
                rightCT = rightCT << 1;
            }
        }
    }
    return hammingDist(leftCT, rightCT);
}

int ADCensus::hammingDist(int64_t a, int64_t b) {
    int result = 0;
    for (int i = 0; i < 64; ++i) {
        if(a >> 63 != b >> 63)
            result++;
        a = a >> 1;
        b = b >> 1;
    }
    return result;
}

double ADCensus::robust(double cost, double lambda) {
    return (1 - exp(-cost / lambda));
}
