#include "adcensus.h"
#include "rgb24Buffer.h"
#include "bufferFactory.h"
#include "tbbWrapper.h"

#include <bmpLoader.h>
#include <calculationStats.h>
#include <cmath>

const double lambdaCT = 10.0;
const double lambdaAD = 30.0;

const int windowWh = 4;
const int windowHh = 3;

ADCensus::ADCensus(QObject *parent) : QObject(parent)
{
}

using corecvs::Matrix;

void ADCensus::constructDisparityMap(QUrl leftImageUrl, QUrl rightImageUrl) {
    // Initialization


//    QImage leftImage(leftImageUrl.toLocalFile());
//    QImage rightImage(rightImageUrl.toLocalFile());
    std::string left  = leftImageUrl.toLocalFile().toStdString();
    std::string right = rightImageUrl.toLocalFile().toStdString();

    cout << "Opening [" << left << " " << right << "]" << endl;

    //RGB24Buffer *leftImage  = BufferFactory::getInstance()->loadRGB24Bitmap(left);
    //RGB24Buffer *rightImage = BufferFactory::getInstance()->loadRGB24Bitmap(right);

    RGB24Buffer *leftImage  = BMPLoader().loadRGB(left);
    RGB24Buffer *rightImage = BMPLoader().loadRGB(right);

    G8Buffer *leftGrayImage  =  leftImage->getChannel(ImageChannel::GRAY);
    G8Buffer *rightGrayImage = rightImage->getChannel(ImageChannel::GRAY);

    int width = leftImage->w;
    int height = leftImage->h;

    BaseTimeStatisticsCollector collector;
    Statistics outerStats;
    outerStats.setValue("H", height);
    outerStats.setValue("W", width);

    //QImage result(width, height, QImage::Format_RGB32);
    RGB24Buffer result(leftImage->getSize());

    auto bestDisparities = Matrix(height, width);
    Matrix minCosts = Matrix(height, width);
    minCosts.fillWith(-1.0);

    // Disparity computation
    outerStats.startInterval();


    AbstractBuffer<uint64_t> *leftCensus  = new AbstractBuffer<uint64_t>(height, width);
    AbstractBuffer<uint64_t> *rightCensus = new AbstractBuffer<uint64_t>(height, width);
    makeCensus(leftGrayImage, leftCensus);
    makeCensus(rightGrayImage, rightCensus);
    outerStats.resetInterval("Making census");

    for (uint i = 0; i < CORE_COUNT_OF(table1); i++)
    {
        table1[i] = robust(i, lambdaCT);
        table2[i] = robust(i, lambdaAD);
    }

    bool parallelDisp = true;

    parallelable_for(0, width / 3,
                     [this, &minCosts, &bestDisparities, &leftImage, &rightImage,
                     &leftCensus, &rightCensus, &collector, height, width, parallelDisp](const BlockedRange<int> &r)
    {
        for (int d = r.begin(); d != r.end(); ++d) {
            Statistics stats;
            stats.startInterval();
            Matrix costs = Matrix(height, width);
            //std::cerr << "Matrix construction: " << stats.helperTimer.usecsToNow() << "\n";
            stats.resetInterval("Matrix construction");

            parallelable_for(windowHh, height - windowHh,
                             [this, &costs, &leftImage, &rightImage, &leftCensus, &rightCensus, d, width](const BlockedRange<int> &r)
            {
                for (int y = r.begin(); y != r.end(); ++y) {
                    RGBColor *im1 = &leftImage->element(y, windowWh + d);
                    RGBColor *im2 = &rightImage->element(y, windowWh);

                    uint64_t *cen1 = &leftCensus->element(y, windowWh + d);
                    uint64_t *cen2 = &rightCensus->element(y, windowWh);

                    int x = windowWh + d;

#ifdef WITH_SSE_UNIMPL
                    for (; x < width - windowWh; x += 8) {
                        FixedVector<Int16x8, 4> c1 = SSEReader8BBBB_DDDD::read((uint32_t *)im1);
                        FixedVector<Int16x8, 4> c2 = SSEReader8BBBB_DDDD::read((uint32_t *)im2);

                        Int16x8 dr = SSEMath::difference(c1[RGBColor::FIELD_R], c2[RGBColor::FIELD_R]);
                        Int16x8 dg = SSEMath::difference(c1[RGBColor::FIELD_G], c2[RGBColor::FIELD_G]);
                        Int16x8 db = SSEMath::difference(c1[RGBColor::FIELD_B], c2[RGBColor::FIELD_B]);

                        Int16x8 sum = dr + dg + db;
                        sum /= Int16x8(4);

                        Int64x2 cen10(&cen1[0]);
                        Int64x2 cen12(&cen1[2]);
                        Int64x2 cen14(&cen1[4]);
                        Int64x2 cen16(&cen1[6]);

                        Int64x2 cen20(&cen2[0]);
                        Int64x2 cen22(&cen2[2]);
                        Int64x2 cen24(&cen2[4]);
                        Int64x2 cen26(&cen2[6]);

                        (cen10 ^ cen20).

                    }
#endif
                    for (; x < width - windowWh; ++x) {
                        uint8_t c_ad = RGBColor::diff(*im1, *im2).brightness();
                        uint8_t c_census = hammingDist(*cen1, *cen2);

                        costs.element(y, x) = robustLUTCen(c_census) + robustLUTAD(c_ad);

                        im1 ++;
                        im2 ++;
                        cen1++;
                        cen2++;
                    }
                }

            }, !parallelDisp
            );

            //std::cerr << "Cost computation: " << stats.helperTimer.usecsToNow() << "\n";
            stats.resetInterval("Cost computation");

            aggregateCosts(&costs, leftImage, windowWh + d, windowHh, width - windowWh, height - windowHh);

            //std::cerr << "Cost aggregation: " << stats.helperTimer.usecsToNow() << "\n";
            stats.resetInterval("Cost aggregation");

            for (int x = windowWh + d; x < width - windowWh; ++x) {
                for (int y = windowHh; y < height - windowHh; ++y) {
                    if(minCosts.element(y, x) < 0 || costs.element(y, x) < minCosts.element(y, x)) {
                        minCosts.element(y, x) = costs.element(y, x);
                        bestDisparities.element(y, x) = d;

                        //result.element(y,x) = RGBColor::gray(bestDisparities.element(y, x) / (double)width * 255 * 3);

                        /*result.setPixel(x, y, QColor((double)bestDisparities.element(y, x) / (double)width * 255 * 3,
                                                      (double)bestDisparities.element(y, x) / (double)width * 255 * 3,
                                                      (double)bestDisparities.element(y, x) / (double)width * 255 * 3));*/
                    }
                }
            }
            //BMPLoader().save("../../result.bmp", &result);

            //std::cerr << "Comparing with previous and saving result: " << stats.helperTimer.usecsToNow() << "\n";
            stats.endInterval("Comparing with previous and saving result");

            //std::cerr << d << "\n";

            collector.addStatistics(stats);

        }
    }, parallelDisp);
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            result.element(y,x) = RGBColor::gray(bestDisparities.element(y, x) / (double)width * 255 * 3);

        }
    }
    BMPLoader().save("../../result.bmp", &result);
    std::cerr << "finished\n";
    outerStats.endInterval("Total");
    collector.addStatistics(outerStats);
    collector.printAdvanced();
    fflush(stdout);
}

#if 0
double ADCensus::costAD(QImage leftImage, QImage rightImage, int x, int y, int disparity) {
    return (double)(
             abs(leftImage.pixelColor(x, y).red() - rightImage.pixelColor(x - disparity, y).red()) +
             abs(leftImage.pixelColor(x, y).green() - rightImage.pixelColor(x - disparity, y).green()) +
             abs(leftImage.pixelColor(x, y).blue() - rightImage.pixelColor(x - disparity, y).blue())
                   ) / 3;
}
#endif

void ADCensus::makeCensus(G8Buffer *image, AbstractBuffer<uint64_t> *census)
{
    if (!image->hasSameSize(census->h, census->w))
        return;
    for (int y = windowHh; y < image->h - windowHh; ++y) {
        for (int x = windowWh; x < image->w - windowWh; ++x) {
            uint8_t center = image->element(y, x);
            for (int i = -windowHh; i < windowHh; ++i) {
                for (int j = -windowWh; j < windowWh; ++j) {
                    if(i != 0 || j != 0) {
                        census->element(y, x) = census->element(y, x) << 1;
                        census->element(y, x) += image->element(y + i, x + j) >= center;
                    }
                }
            }
        }
    }
}

double ADCensus::costCensus(RGB24Buffer* leftImage, RGB24Buffer* rightImage, int x, int y, int disparity) {
    //int64_t leftCT = 0;
    //int64_t rightCT = 0;
    int leftCenter  = leftImage->element(y, x).brightness();
    int rightCenter = rightImage->element(y, x - disparity).brightness();
    int result = 0;

    for (int i = -windowHh; i < windowHh; ++i) {
        for (int j = -windowWh; j < windowWh; ++j) {
            if(i != 0 || j != 0) {
                result += ((leftImage->element(y + i, x + j).brightness() >= leftCenter) !=
                           (rightImage->element(y + i, x - disparity + j).brightness() >= rightCenter));

                /*
                leftCT += (leftImage->element(y + i, x + j).brightness() >= leftCenter ? 1 : 0);

                rightCT += (rightImage->element(y + i, x - disparity + j).brightness() >= rightCenter ? 1 : 0);

                leftCT = leftCT << 1;
                rightCT = rightCT << 1;
                */
            }
        }
    }
    return (double)result; //hammingDist(leftCT, rightCT);
}

inline int ADCensus::hammingDist(uint64_t a, uint64_t b) {
#ifdef OLD_STYLE
    int result = 0;
    uint64_t diff = a ^ b;
    while(diff != 0) {
        result++;
        diff = diff & (diff - 1);
    }
    /*
    while(diff != 0) {
        result += (diff & 1);
        diff = diff >> 1;
    }
    */
    return result;
#else
    return _mm_popcnt_u64(a^b);
#endif
}

double ADCensus::robust(double cost, double lambda) {
    return (1 - exp(-cost / lambda));
}


double ADCensus::robustLUTCen(uint8_t in) {
    return table1[in];
}

double ADCensus::robustLUTAD(uint8_t in) {
    return table2[in];
}


void ADCensus::aggregateCosts(corecvs::Matrix *costs, RGB24Buffer *image, int leftBorder, int topBorder, int width, int height) {
    auto rlAggregation = Matrix(height, width);
    for (int y = topBorder; y < height; ++y) {
        for (int x = leftBorder; x < width; ++x) {
            RGBColor currentPixel = image->element(y, x);
            int dx = 1;
            int i;
            rlAggregation.element(y, x) = costs->element(y, x);
            // Right arm
            for (i = 1; x + i < width; ++i) {
                RGBColor toAddPixel = image->element(y, x + i);
                RGBColor prevPixel = image->element(y, x + (i - 1));

                if(!fitsForAggregation(i, currentPixel, toAddPixel, prevPixel))
                    break;
                rlAggregation.element(y, x) += costs->element(y, x + i);
            }
            dx += i - 1;
            // Left arm
            for (i = 1; x - i > leftBorder; ++i) {
                RGBColor toAddPixel = image->element(y, x - i);
                RGBColor prevPixel = image->element(y, x - (i - 1));

                if(!fitsForAggregation(i, currentPixel, toAddPixel, prevPixel))
                    break;
                rlAggregation.element(y, x) += costs->element(y, x - i);
            }
            dx += i - 1;
            rlAggregation.element(y, x) /= dx;
        }
    }

    for (int y = topBorder; y < height; ++y) {
        for (int x = leftBorder; x < width; ++x) {
            RGBColor currentPixel = image->element(y, x);
            int dy = 1;
            int i;
            // Top arm
            for (i = 1; y + i < height; ++i) {
                RGBColor toAddPixel = image->element(y + i, x);
                RGBColor prevPixel = image->element(y + (i - 1), x);

                if(!fitsForAggregation(i, currentPixel, toAddPixel, prevPixel))
                    break;
                costs->element(y, x) += rlAggregation.element(y + i, x);
            }
            dy += i - 1;
            // Bottom arm
            for (i = 1; y - i > topBorder; ++i) {
                RGBColor toAddPixel = image->element(y - i, x);
                RGBColor prevPixel = image->element(y - (i - 1), x);

                if(!fitsForAggregation(i, currentPixel, toAddPixel, prevPixel))
                    break;
                costs->element(y, x) += rlAggregation.element(y - i, x);
            }
            dy += i - 1;
            costs->element(y, x) /= dy;
        }
    }
}
