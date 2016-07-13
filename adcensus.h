#ifndef ADCENSUS_H
#define ADCENSUS_H

#include <QObject>
#include <QQuickImageProvider>
#include <QImage>

class ADCensus : public QObject
{
    Q_OBJECT
public:
    explicit ADCensus(QObject *parent = 0);

signals:

public slots:
    void constructDisparityMap(QUrl leftImageUrl, QUrl rightImageUrl);

private:
};

#endif // ADCENSUS_H
