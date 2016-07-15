#include "adcensus.h"
#include <cmath>

const double lambdaCT = 10.0;
const double lambdaAD = 30.0;

const int windowWh = 4;
const int windowHh = 3;

const int maxAggregationArmLen = 34;
const int avgAggregationArmLen = 17;
const int anyAggregationArmColorThreshold = 20;
const int maxAggregationArmColorThreshold = 6;

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
    auto bestDisparities = corecvs::Matrix(height, width);
    auto minCosts = corecvs::Matrix(height, width);
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y)
            minCosts.element(y, x) = -1.0;

    // Disparity computation
    for (int d = 0; d < width / 3; ++d) {
        auto costs = corecvs::Matrix(height, width);
        for (int x = windowWh + d; x < width - windowWh; ++x) {
            for (int y = windowHh; y < height - windowHh; ++y) {
                auto c_ad = costAD(leftImage, rightImage, x, y, d);
                auto c_census = costCensus(leftImage, rightImage, x, y, d);
                costs.element(y, x) = robust(c_census, lambdaCT) + robust(c_ad, lambdaAD);
            }
        }
        aggregateCosts(&costs, leftImage, windowWh + d, windowHh, width - windowWh, height - windowHh);
        for (int x = windowWh + d; x < width - windowWh; ++x) {
            for (int y = windowHh; y < height - windowHh; ++y) {
                if(minCosts.element(y, x) < 0 || costs.element(y, x) < minCosts.element(y, x)) {
                    minCosts.element(y, x) = costs.element(y, x);
                    bestDisparities.element(y, x) = d;
                    result.setPixelColor(x, y, QColor((double)bestDisparities.element(y, x) / (double)width * 255 * 3,
                                                      (double)bestDisparities.element(y, x) / (double)width * 255 * 3,
                                                      (double)bestDisparities.element(y, x) / (double)width * 255 * 3));
                }
            }
        }
        result.save("../../result.png");
        std::cerr << d << "\n";
    }
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            result.setPixelColor(x, y, QColor((double)bestDisparities.element(y, x) / (double)width * 255 * 3,
                                              (double)bestDisparities.element(y, x) / (double)width * 255 * 3,
                                              (double)bestDisparities.element(y, x) / (double)width * 255 * 3));
        }
    }
    result.save("../../result.png");
    std::cerr << "finished\n";
}

double ADCensus::costAD(QImage leftImage, QImage rightImage, int x, int y, int disparity) {
    return (double)(
             abs(leftImage.pixelColor(x, y).red() - rightImage.pixelColor(x - disparity, y).red()) +
             abs(leftImage.pixelColor(x, y).green() - rightImage.pixelColor(x - disparity, y).green()) +
             abs(leftImage.pixelColor(x, y).blue() - rightImage.pixelColor(x - disparity, y).blue())
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
    for (uint i = 0; i < sizeof(a) * 8; ++i) {
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

void ADCensus::aggregateCosts(corecvs::Matrix *costs, QImage image, int leftBorder, int topBorder, int width, int height) {
    auto rlAggregation = corecvs::Matrix(height, width);
    for (int x = leftBorder; x < width; ++x) {
        for (int y = topBorder; y < height; ++y) {
            QColor currentPixel = image.pixelColor(x, y);
            int dx = 1;
            rlAggregation.element(y, x) = costs->element(y, x);
            int i;
            // Right arm
            for (i = 1; x + i < width; ++i) {
                if(i >= maxAggregationArmLen)
                    break;
                if(colorDifference(currentPixel, image.pixelColor(x + i, y)) >= anyAggregationArmColorThreshold ||
                   colorDifference(image.pixelColor(x + i, y), image.pixelColor(x + i - 1, y)) >= anyAggregationArmColorThreshold)
                    break;
                if(i > avgAggregationArmLen &&
                   colorDifference(currentPixel, image.pixelColor(x + i, y)) >= maxAggregationArmColorThreshold)
                    break;
                rlAggregation.element(y, x) += costs->element(y, x + i);
            }
            dx += i - 1;
            // Left arm
            for (i = 1; x - i > leftBorder; ++i) {
                if(i >= maxAggregationArmLen)
                    break;
                if(colorDifference(currentPixel, image.pixelColor(x - i, y)) >= anyAggregationArmColorThreshold ||
                   colorDifference(image.pixelColor(x - i, y), image.pixelColor(x - i + 1, y)) >= anyAggregationArmColorThreshold)
                    break;
                if(i > avgAggregationArmLen &&
                   colorDifference(currentPixel, image.pixelColor(x - i, y)) >= maxAggregationArmColorThreshold)
                    break;
                rlAggregation.element(y, x) += costs->element(y, x - i);
            }
            dx += i - 1;
            rlAggregation.element(y, x) /= dx;

            int dy = 1;
            // Top arm
            for (i = 1; y + i < height; ++i) {
                if(i >= maxAggregationArmLen)
                    break;
                if(colorDifference(currentPixel, image.pixelColor(x, y + i)) >= anyAggregationArmColorThreshold ||
                   colorDifference(image.pixelColor(x, y + i), image.pixelColor(x, y + i - 1)) >= anyAggregationArmColorThreshold)
                    break;
                if(i > avgAggregationArmLen &&
                   colorDifference(currentPixel, image.pixelColor(x, y + i)) >= maxAggregationArmColorThreshold)
                    break;
                costs->element(y, x) += rlAggregation.element(y + i, x);
            }
            dy += i - 1;
            // Bottom arm
            for (i = 1; y - i > topBorder; ++i) {
                if(i >= maxAggregationArmLen)
                    break;
                if(colorDifference(currentPixel, image.pixelColor(x, y - i)) >= anyAggregationArmColorThreshold ||
                   colorDifference(image.pixelColor(x, y - i), image.pixelColor(x, y - i + 1)) >= anyAggregationArmColorThreshold)
                    break;
                if(i > avgAggregationArmLen &&
                   colorDifference(currentPixel, image.pixelColor(x, y - i)) >= maxAggregationArmColorThreshold)
                    break;
                costs->element(y, x) += rlAggregation.element(y - i, x);
            }
            dy += i - 1;
            costs->element(y, x) /= dy;
        }
    }
}
