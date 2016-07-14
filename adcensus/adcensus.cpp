#include "adcensus.h"
#include <cmath>

const double lambdaCT = 1.0;
const double lambdaAD = 3.0;

const int windowWh = 4;
const int windowHh = 3;

ADCensus::ADCensus(QObject *parent) : QObject(parent)
{
}

void ADCensus::constructDisparityMap(QUrl leftImageUrl, QUrl rightImageUrl) {
    // Initialization
    QImage leftImage(leftImageUrl.toLocalFile());
    QImage rightImage(rightImageUrl.toLocalFile());
    int width = leftImage.width();
    int height = leftImage.height();
    QImage result(width, height, QImage::Format_RGB32);

    // Simple cost-based disparity computation
    for (int x = windowWh; x < width - windowWh; ++x) {
        for (int y = windowHh; y < height - windowHh; ++y) {
            double minCost = -1;
            int bestDisparity = 0;
            for (int d = 0; d < x - windowWh && d < width / 3; ++d) {
                auto c_ad = costAD(leftImage, rightImage, x, y, d);
                auto c_census = costCensus(leftImage, rightImage, x, y, d);
                auto c_full = robust(c_census, lambdaCT) + robust(c_ad, lambdaAD);
                if(minCost < 0 || c_full < minCost) {
                    minCost = c_full;
                    bestDisparity = d;
                }
            }
            result.setPixelColor(x, y, QColor((double)bestDisparity / (double)width * 255 * 3,
                                              (double)bestDisparity / (double)width * 255 * 3,
                                              (double)bestDisparity / (double)width * 255 * 3));
        }
        if(x % 10 == 0)
            result.save("../../result.png");
        std::cerr << x << "\n";
    }
    result.save("../../result.png");
    std::cerr << "finished\n";
}

double ADCensus::costAD(QImage leftImage, QImage rightImage, int x, int y, int disparity) {
    return (
            (double)abs(leftImage.pixelColor(x, y).red() - rightImage.pixelColor(x - disparity, y).red()) +
            (double)abs(leftImage.pixelColor(x, y).green() - rightImage.pixelColor(x - disparity, y).green()) +
            (double)abs(leftImage.pixelColor(x, y).blue() - rightImage.pixelColor(x - disparity, y).blue())
           ) / 3;
}

double ADCensus::costCensus(QImage leftImage, QImage rightImage, int x, int y, int disparity) {
    int64_t leftCT = 0;
    int64_t rightCT = 0;
    int leftCenter = qGray(leftImage.pixel(x, y));
    int rightCenter = qGray(rightImage.pixel(x - disparity, y));

    for (int i = -windowWh; i < windowWh; ++i) {
        for (int j = -windowHh; j < windowHh; ++j) {
            if(i != 0 || j != 0) {
                leftCT += (qGray(leftImage.pixel(x + i, y + j)) >= leftCenter ? 1 : 0);
                rightCT += (qGray(rightImage.pixel(x - disparity + i, y + j)) >= rightCenter ? 1 : 0);
                leftCT = leftCT << 1;
                rightCT = rightCT << 1;
            }
        }
    }
    return (double)hammingDist(leftCT, rightCT);
}

int ADCensus::hammingDist(int64_t a, int64_t b) {
    int result = 0;
    for (int i = 0; i < 64; ++i) {
        if((a | 1) != (b | 1))
            result++;
        a = a >> 1;
        b = b >> 1;
    }
    return result;
}

double ADCensus::robust(double cost, double lambda) {
    return (1 - exp(-cost / lambda));
}
