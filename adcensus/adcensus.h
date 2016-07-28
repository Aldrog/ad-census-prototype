#ifndef ADCENSUS_H
#define ADCENSUS_H

#include <QColor>

#include <QObject>
#include <QImage>
#include <QUrl>
#include <algorithm>

#include "rgb24Buffer.h"
#include "matrix.h"

class ADCensus : public QObject
{
    Q_OBJECT
public:
    explicit ADCensus(QObject *parent = 0);

    template<typename pixel, typename grayPixel>
    AbstractBuffer<int32_t> constructDisparityMap(const AbstractBuffer<pixel> *leftImage, const AbstractBuffer<pixel> *rightImage,
                                                  const AbstractBuffer<grayPixel> *leftGrayImage, const AbstractBuffer<grayPixel> *rightGrayImage);
signals:

public slots:
    void disparityMapFromGrayscale(QUrl leftImageUrl, QUrl rightImageUrl);
    void disparityMapFromRGB(QUrl leftImageUrl, QUrl rightImageUrl);

private:

#define COST_TYPE uint16_t

    const int maxAggregationArmLen = 34;
    const int avgAggregationArmLen = 17;
    const int anyAggregationArmColorThreshold = 20;
    const int maxAggregationArmColorThreshold = 6;

    COST_TYPE table1[256];
    COST_TYPE table2[256];

    COST_TYPE robustLUTCen(uint8_t in);
    COST_TYPE robustLUTAD(uint8_t in);

    AbstractBuffer<Vector4d<uint8_t>> aggregationCrosses;

    template<typename pixel>
    void makeAggregationCrosses(const AbstractBuffer<pixel> *image);

    AbstractBuffer<bool> bordersLeft;
    AbstractBuffer<bool> bordersTop;

    template<typename pixel>
    void findBorderPixels(const AbstractBuffer<pixel> *image);

    template<int sx, int sy, typename pixel>
    int makeArm(const AbstractBuffer<pixel> *image, int x, int y);

    template<typename pixel>
    void makeCensus(const AbstractBuffer<pixel> *image, AbstractBuffer<int64_t> &census);

    uint8_t hammingDist(int64_t a, int64_t b);

    COST_TYPE robust(uint8_t cost, double lambda);

    void aggregateCosts(AbstractBuffer<COST_TYPE> *costs, int leftBorder, int topBorder, int width, int height);

    uint8_t colorDifference(const RGBColor &a, const RGBColor &b);
    uint8_t colorDifference(const uint16_t &a, const uint16_t &b);

    uint8_t costAD(const RGBColor &a, const RGBColor &b);
    uint8_t costAD(const uint16_t &a, const uint16_t &b);

    template<typename pixel>
    bool fitsForAggregation(int len, const pixel &current, const pixel &toCheck);
};

#endif // ADCENSUS_H
