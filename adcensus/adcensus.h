#ifndef ADCENSUS_H
#define ADCENSUS_H

#include <QObject>
#include <QImage>
#include <QUrl>
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
    corecvs::Matrix costAD(QImage leftImage, QImage rightImage);
    corecvs::Matrix costCensus(QImage leftImage, QImage rightImage);
};

#endif // ADCENSUS_H
