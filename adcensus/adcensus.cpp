#include "adcensus.h"
#include "rgb24Buffer.h"
#include "bufferFactory.h"
#include "tbbWrapper.h"

#include <bmpLoader.h>
#include <calculationStats.h>
#include <cmath>

#define DIR_RIGHT 0
#define DIR_LEFT 1
#define DIR_DOWN 2
#define DIR_UP 3

const double lambdaCT = 10.0;
const double lambdaAD = 30.0;

const int windowWh = 4;
const int windowHh = 3;

const COST_TYPE robustPrecision = 127;

ADCensus::ADCensus(QObject *parent) : QObject(parent)
{
}

using corecvs::Matrix;

void ADCensus::disparityMapFromGrayscale(QUrl leftImageUrl, QUrl rightImageUrl) {
    std::string left  = leftImageUrl.toLocalFile().toStdString();
    std::string right = rightImageUrl.toLocalFile().toStdString();

    cout << "Opening [" << left << " " << right << "]" << endl;

    G12Buffer *leftImage  = BMPLoader().loadG12(left);
    G12Buffer *rightImage = BMPLoader().loadG12(right);
    G12Buffer *leftGray = new G12Buffer(leftImage);
    G12Buffer *rightGray = new G12Buffer(rightImage);

    G8Buffer *result = constructDisparityMap(leftImage, rightImage, leftGray, rightGray);
    BMPLoader().save("../../result.bmp", result);

    delete leftImage;
    delete rightImage;
    delete leftGray;
    delete rightGray;
    delete result;
}

void ADCensus::disparityMapFromRGB(QUrl leftImageUrl, QUrl rightImageUrl) {
    std::string left  = leftImageUrl.toLocalFile().toStdString();
    std::string right = rightImageUrl.toLocalFile().toStdString();

    cout << "Opening [" << left << " " << right << "]" << endl;

    RGB24Buffer *leftImage  = BMPLoader().loadRGB(left);
    RGB24Buffer *rightImage = BMPLoader().loadRGB(right);
    G8Buffer *leftGray = leftImage->getChannel(ImageChannel::GRAY);
    G8Buffer *rightGray = rightImage->getChannel(ImageChannel::GRAY);

    G8Buffer *result = constructDisparityMap(leftImage, rightImage, leftGray, rightGray);
    BMPLoader().save("../../result.bmp", result);

    delete leftImage;
    delete rightImage;
    delete leftGray;
    delete rightGray;
    delete result;
}

template<typename pixel, typename grayPixel>
G8Buffer *ADCensus::constructDisparityMap(AbstractBuffer<pixel> *leftImage, AbstractBuffer<pixel> *rightImage,
                                          AbstractBuffer<grayPixel> *leftGrayImage, AbstractBuffer<grayPixel> *rightGrayImage) {
    // Initialization

    int width = leftImage->w;
    int height = leftImage->h;

    BaseTimeStatisticsCollector collector;
    Statistics outerStats;
    outerStats.setValue("H", height);
    outerStats.setValue("W", width);

    G8Buffer *result = new G8Buffer(leftImage->getSize());

    auto bestDisparities = AbstractBuffer<uint32_t>(height, width);
    AbstractBuffer<COST_TYPE> minCosts = AbstractBuffer<COST_TYPE>(height, width);
    minCosts.fillWith(-1);

    // Disparity computation
    outerStats.startInterval();

    AbstractBuffer<uint64_t> *leftCensus  = new AbstractBuffer<uint64_t>(height, width);
    AbstractBuffer<uint64_t> *rightCensus = new AbstractBuffer<uint64_t>(height, width);
    makeCensus(leftGrayImage, leftCensus);
    makeCensus(rightGrayImage, rightCensus);
    outerStats.resetInterval("Making census");

    makeAggregationCrosses(leftImage);
    outerStats.resetInterval("Making aggregation crosses");

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
            AbstractBuffer<COST_TYPE> costs = AbstractBuffer<COST_TYPE>(height, width);
            //std::cerr << "Matrix construction: " << stats.helperTimer.usecsToNow() << "\n";
            stats.resetInterval("Matrix construction");

            parallelable_for(windowHh, height - windowHh,
                             [this, &costs, &leftImage, &rightImage, &leftCensus, &rightCensus, d, width](const BlockedRange<int> &r)
            {
                for (int y = r.begin(); y != r.end(); ++y) {
                    auto *im1 = &leftImage->element(y, windowWh + d);
                    auto *im2 = &rightImage->element(y, windowWh);

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
                        uint8_t c_ad = costAD(*im1, *im2);
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

            stats.resetInterval("Cost computation");

            aggregateCosts(&costs, windowWh + d, windowHh, width - windowWh, height - windowHh);

            stats.resetInterval("Cost aggregation");

            for (int x = windowWh + d; x < width - windowWh; ++x) {
                for (int y = windowHh; y < height - windowHh; ++y) {
                    if(costs.element(y, x) < minCosts.element(y, x)) {
                        minCosts.element(y, x) = costs.element(y, x);
                        bestDisparities.element(y, x) = d;

                        //result.element(y,x) = (bestDisparities.element(y, x) / (double)width * 255 * 3);
                    }
                }
            }
            //BMPLoader().save("../../result.bmp", result);

            stats.endInterval("Comparing with previous minimum");
            collector.addStatistics(stats);

        }
    }, parallelDisp);
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            result->element(y,x) = (bestDisparities.element(y, x) / (double)width * 255 * 3);
        }
    }
    outerStats.endInterval("Total");
    collector.addStatistics(outerStats);
    collector.printAdvanced();
    fflush(stdout);

    return result;
}

template<typename pixel>
void ADCensus::makeCensus(AbstractBuffer<pixel> *image, AbstractBuffer<uint64_t> *census)
{
    if (!image->hasSameSize(census->h, census->w))
        return;
    for (int y = windowHh; y < image->h - windowHh; ++y) {
        for (int x = windowWh; x < image->w - windowWh; ++x) {
            pixel center = image->element(y, x);
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

template<typename pixel>
void ADCensus::makeAggregationCrosses(AbstractBuffer<pixel> *image) {
    int width = image->w;
    int height = image->h;
    aggregationCrosses = AbstractBuffer<Vector4d<uint8_t>>(height, width);
    for (int y = windowHh; y < height - windowHh; ++y) {
        for (int x = windowWh; x < width - windowWh; ++x) {
            aggregationCrosses.element(y, x) = Vector4d<uint8_t>
                        (
                            makeArm<1, 0>(image, x, y),
                            makeArm<-1, 0>(image, x, y),
                            makeArm<0, 1>(image, x, y),
                            makeArm<0, -1>(image, x, y)
                        );
        }
    }
}

template<int sx, int sy, typename pixel>
int ADCensus::makeArm(AbstractBuffer<pixel> *image, int x, int y) {
    pixel currentPixel = image->element(y, x);

    int i;
    for (i = 1; ; ++i) {
        if(x + i * sx >= image->w - windowWh ||
           x + i * sx < windowWh ||
           y + i * sy >= image->h - windowHh ||
           y + i * sy < windowHh)
            break;
        pixel toAddPixel = image->element(y + i * sy, x + i * sx);
        pixel prevPixel = image->element(y + (i - 1) * sy, x + (i - 1) * sx);

        if(!fitsForAggregation(i, currentPixel, toAddPixel, prevPixel))
            break;
    }
    return i - 1;
}

inline uint8_t ADCensus::hammingDist(uint64_t a, uint64_t b) {
    return _mm_popcnt_u64(a^b);
}

COST_TYPE ADCensus::robust(uint8_t cost, double lambda) {
    return (1 - exp(-cost / lambda)) * robustPrecision;
}


COST_TYPE ADCensus::robustLUTCen(uint8_t in) {
    return table1[in];
}

COST_TYPE ADCensus::robustLUTAD(uint8_t in) {
    return table2[in];
}

void ADCensus::aggregateCosts(AbstractBuffer<COST_TYPE> *costs, int leftBorder, int topBorder, int width, int height) {
    AbstractBuffer<COST_TYPE> *rlAggregation = new AbstractBuffer<COST_TYPE>(height, width);
    for (int y = topBorder; y < height; ++y) {
        for (int x = leftBorder; x < width; ++x) {
            int len = 0;
            for (int curX = std::max(x - aggregationCrosses.element(y, x)[DIR_LEFT], leftBorder);
                     curX <= std::min(x + aggregationCrosses.element(y, x)[DIR_RIGHT], width - 1);
                     ++curX) {
                rlAggregation->element(y, x) += costs->element(y, curX);
                len++;
            }
            rlAggregation->element(y, x) /= len;
        }
    }
    costs->fillWith(0);
    for (int y = topBorder; y < height; ++y) {
        for (int x = leftBorder; x < width; ++x) {
            int len = 0;
            for (int curY = std::max(y - aggregationCrosses.element(y, x)[DIR_UP], topBorder);
                     curY <= std::min(y + aggregationCrosses.element(y, x)[DIR_DOWN], height - 1);
                     ++curY) {
                costs->element(y, x) += rlAggregation->element(curY, x);
                len++;
            }
            costs->element(y, x) /= len;
        }
    }
    delete rlAggregation;
}
