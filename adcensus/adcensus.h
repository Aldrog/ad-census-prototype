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
    G8Buffer *constructDisparityMap(AbstractBuffer<pixel> *leftImage, AbstractBuffer<pixel> *rightImage,
                                    AbstractBuffer<grayPixel> *leftGrayImage, AbstractBuffer<grayPixel> *rightGrayImage);
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

    corecvs::AbstractBuffer<corecvs::Vector4d<uint8_t>> aggregationCrosses;

    template<typename pixel>
    void makeAggregationCrosses(corecvs::AbstractBuffer<pixel> *image);

    AbstractBuffer<bool> *borderLeft;
    AbstractBuffer<bool> *borderTop;

    template<typename pixel>
    void findBorderPixels(AbstractBuffer<pixel> *image);

    template<int sx, int sy, typename pixel>
    int makeArm(corecvs::AbstractBuffer<pixel> *image, int x, int y);

    template<typename pixel>
    void makeCensus(AbstractBuffer<pixel> *image, corecvs::AbstractBuffer<int64_t> *census);

    uint8_t hammingDist(int64_t a, int64_t b);

    COST_TYPE robust(uint8_t cost, double lambda);

    void aggregateCosts(AbstractBuffer<COST_TYPE> *costs, int leftBorder, int topBorder, int width, int height);

    inline int colorDifference(const RGBColor &a, const RGBColor &b) {
        return RGBColor::diff(a, b).maximum();
    }

    inline int colorDifference(const uint16_t &a, const uint16_t &b) {
        return abs(a - b);
    }

    inline uint8_t costAD(const RGBColor &a, const RGBColor &b) {
        return RGBColor::diff(a, b).brightness();
    }

    inline uint8_t costAD(const uint16_t &a, const uint16_t &b) {
        return abs((a >> 4) - (b >> 4));
    }

    inline bool fitsForAggregation(int len, RGBColor current, RGBColor toCheck) {
        return (
                    len < maxAggregationArmLen &&
                    colorDifference(current, toCheck) < anyAggregationArmColorThreshold &&
                    (len < avgAggregationArmLen || colorDifference(current, toCheck) < maxAggregationArmColorThreshold)
               );
    }

    inline bool fitsForAggregation(int len, uint16_t current, uint16_t toCheck) {
        return (
                    len < maxAggregationArmLen &&
                    abs(current - toCheck) < anyAggregationArmColorThreshold &&
                    (len < avgAggregationArmLen || abs(current - toCheck) < maxAggregationArmColorThreshold)
               );
    }
};

#endif // ADCENSUS_H
