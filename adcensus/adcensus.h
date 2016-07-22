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

signals:

public slots:
    void constructDisparityMap(QUrl leftImageUrl, QUrl rightImageUrl);

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

    void makeAggregationCrosses(corecvs::RGB24Buffer *image);
    template<int sx, int sy>
    int makeArm(corecvs::RGB24Buffer *image, int x, int y);

    double costAD(QImage leftImage, QImage rightImage, int x, int y, int disparity);
    double costCensus(corecvs::RGB24Buffer *leftImage, corecvs::RGB24Buffer *rightImage, int x, int y, int disparity);
    void makeCensus(corecvs::G8Buffer *image, corecvs::AbstractBuffer<uint64_t> *census);
    uint8_t hammingDist(uint64_t a, uint64_t b);
    COST_TYPE robust(uint8_t cost, double lambda);
    void aggregateCosts(AbstractBuffer<COST_TYPE> *costs, corecvs::RGB24Buffer *image, int leftBorder, int topBorder, int width, int height);
    void aggregateCosts(AbstractBuffer<COST_TYPE> *costs, int leftBorder, int topBorder, int width, int height);

    template <int sx, int sy>
    COST_TYPE sumArm(corecvs::AbstractBuffer<COST_TYPE> *costs, corecvs::RGB24Buffer *image, int *length,
                  int x, int y, int leftBorder, int topBorder, int width, int height);

    inline int colorDifference(QColor a, QColor b) {
        return std::max( std::max(
                            abs(a.red() - b.red()),
                            abs(a.green() - b.green()) ),
                            abs(a.blue() - b.blue()) );
    }

    inline int colorDifference(const RGBColor &a, const RGBColor &b) {
        return RGBColor::diff(a,b).maximum();
    }

    inline bool fitsForAggregation(int len, RGBColor current, RGBColor toCheck, RGBColor previous) {
        return (
                    len < maxAggregationArmLen &&
                    colorDifference(current, toCheck) < anyAggregationArmColorThreshold &&
                    colorDifference(toCheck, previous) < anyAggregationArmColorThreshold &&
                    (len < avgAggregationArmLen || colorDifference(current, toCheck) < maxAggregationArmColorThreshold)
               );
    }
};

#endif // ADCENSUS_H
