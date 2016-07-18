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
    double costAD(QImage leftImage, QImage rightImage, int x, int y, int disparity);
    double costCensus(corecvs::RGB24Buffer *leftImage, corecvs::RGB24Buffer *rightImage, int x, int y, int disparity);
    int hammingDist(int64_t a, int64_t b);
    double robust(double cost, double lambda);
    void aggregateCosts(corecvs::Matrix *costs, corecvs::RGB24Buffer * image, int leftBorder, int topBorder, int width, int height);

    inline int colorDifference(QColor a, QColor b) {
        return std::max( std::max(
                            abs(a.red() - b.red()),
                            abs(a.green() - b.green()) ),
                            abs(a.blue() - b.blue()) );
    }

    inline int colorDifference(const RGBColor &a, const RGBColor &b) {
        return RGBColor::diff(a,b).maximum();
    }
};

#endif // ADCENSUS_H
