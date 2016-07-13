/**
 * \file blurProcessor.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 19, 2010
 * \author alexander
 */
#ifndef BLURPROCESSOR_H_
#define BLURPROCESSOR_H_

#include "global.h"

#include "integralBuffer.h"
#include "g12Buffer.h"

namespace corecvs {

class BlurProcessor
{
public:
    static const int BLUR_RADIUS = 2;

    static void prepareBlurBuffers(G12IntegralBuffer *integral, G12Buffer* blur[4]);

    static void prepareBlurBuffers(G12IntegralBuffer *integral,
            unsigned blurH,
            unsigned blurW,
            unsigned stepH,
            unsigned stepW,
            G12Buffer** blur);

#ifdef WITH_SSE
    static void prepareBlurBuffersSSE(G12IntegralBuffer *integral, G12Buffer* blur[4]);
#endif

    static const int xshift[4];
    static const int yshift[4];
};


} //namespace corecvs

#endif /* BLURPROCESSOR_H_ */

