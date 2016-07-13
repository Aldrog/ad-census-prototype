#ifndef RGB_COLOR_PARAMETERS_H_
#define RGB_COLOR_PARAMETERS_H_
/**
 * \file rgbColorParameters.h
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 */

#include "reflection.h"
#include "defaultSetter.h"
#include "printerVisitor.h"

/*
 *  Embed includes.
 */
/*
 *  Additional includes for Composite Types.
 */

using namespace corecvs;

/*
 *  Additional includes for Pointer Types.
 */

namespace corecvs {
}
/*
 *  Additional includes for enum section.
 */

/**
 * \brief rgb Color Parameters 
 * rgb Color Parameters 
 **/
class RgbColorParameters : public BaseReflection<RgbColorParameters>
{
public:
    enum FieldId {
        R_ID,
        G_ID,
        B_ID,
        RGB_COLOR_PARAMETERS_FIELD_ID_NUM
    };

    /** Section with variables */

    /** 
     * \brief R 
     * R 
     */
    int mR;

    /** 
     * \brief G 
     * G 
     */
    int mG;

    /** 
     * \brief B 
     * B 
     */
    int mB;

    /** Static fields init function, this is used for "dynamic" field initialization */ 
    static int staticInit();

    /** Section with getters */
    const void *getPtrById(int fieldId) const
    {
        return (const unsigned char *)(this) + fields()[fieldId]->offset;
    }
    int r() const
    {
        return mR;
    }

    int g() const
    {
        return mG;
    }

    int b() const
    {
        return mB;
    }

    /* Section with setters */
    void setR(int r)
    {
        mR = r;
    }

    void setG(int g)
    {
        mG = g;
    }

    void setB(int b)
    {
        mB = b;
    }

    /* Section with embedded classes */
    /* visitor pattern - http://en.wikipedia.org/wiki/Visitor_pattern */
template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(mR,                         static_cast<const IntField *>     (fields()[R_ID]));
        visitor.visit(mG,                         static_cast<const IntField *>     (fields()[G_ID]));
        visitor.visit(mB,                         static_cast<const IntField *>     (fields()[B_ID]));
    }

    RgbColorParameters()
    {
        DefaultSetter setter;
        accept(setter);
    }

    RgbColorParameters(
          int r
        , int g
        , int b
    )
    {
        mR = r;
        mG = g;
        mB = b;
    }

    friend ostream& operator << (ostream &out, RgbColorParameters &toSave)
    {
        PrinterVisitor printer(out);
        toSave.accept<PrinterVisitor>(printer);
        return out;
    }

    void print ()
    {
        cout << *this;
    }
};
#endif  //RGB_COLOR_PARAMETERS_H_
