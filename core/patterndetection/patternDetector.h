#ifndef PATTERNDETECTOR
#define PATTERNDETECTOR

#include "calculationStats.h"

#include "selectableGeometryFeatures.h"
#include "g8Buffer.h"
#include "rgb24Buffer.h"


/**
 *   Generic interface for pattern detecting
 **/
class PatternDetector
{
public:
    /**
     * Calls pattern detector on grayscale buffer
     **/
    virtual bool detectPattern(corecvs::G8Buffer    &buffer) = 0;
    /**
     * (by default) calls detectPattern(G8Buffer) with transformed image
     **/
    virtual bool detectPattern(corecvs::RGB24Buffer &buffer);
    /**
     * Returns previously detected points as ObservationList
     **/
    virtual void getPointData(corecvs::ObservationList &observations) = 0;
    /**
     *  Returns previously detected points as SGF (by default - brutforces ObservationList
     **/
    virtual void getPointData(corecvs::SelectableGeometryFeatures &features);
    virtual ~PatternDetector() {}

    virtual void setStatistics(corecvs::Statistics * /*stats*/ = NULL) {}
};

#endif
