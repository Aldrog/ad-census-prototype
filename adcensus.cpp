#include "adcensus.h"
#include <QDebug>

ADCensus::ADCensus(QObject *parent) : QObject(parent)
{
}

void ADCensus::constructDisparityMap(QUrl leftImageUrl, QUrl rightImageUrl) {
    QImage leftImage(leftImageUrl.toLocalFile());
    QImage rightImage(rightImageUrl.toLocalFile());
    QImage result(leftImage.width(), leftImage.height(), QImage::Format_Grayscale8);
    // Computations
    result.fill(Qt::gray);
    result.save("../result.png");
    qDebug() << "finished";
}
