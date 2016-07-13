#ifndef POLYGONS_H_
#define POLYGONS_H_

/**
 * \file polygons.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 14, 2010
 * \author alexander
 */


#include <vector>
#include <algorithm>
#include <ostream>

#include "vector3d.h"
#include "generated/axisAlignedBoxParameters.h"
#include "line.h"
#include "rectangle.h"

namespace corecvs {

using std::vector;

template<typename PointType>
class GenericTriangle
{
public:
    PointType p[3];

    PointType &p1() {return p[0];}
    PointType &p2() {return p[1];}
    PointType &p3() {return p[2];}

    const PointType &p1() const {return p[0];}
    const PointType &p2() const {return p[1];}
    const PointType &p3() const {return p[2];}

    GenericTriangle() {}

    GenericTriangle(const PointType _p1, const PointType _p2, const PointType _p3)
    {
        p1() = _p1;
        p2() = _p2;
        p3() = _p3;
    }

    Plane3d getPlane() const
    {
        return Plane3d::FromPoints(p1(), p2(), p3());
    }

    Vector3dd getNormal() const
    {
        return Plane3d::NormalFromPoints(p1(), p2(), p3());
    }

    /** NOTE: This could swap the normal **/
    void sortByY() {
        if (p1().y() > p2().y()) std::swap(p1(), p2());
        if (p2().y() > p3().y()) std::swap(p2(), p3());
        if (p1().y() > p2().y()) std::swap(p1(), p2());
    }

};

typedef GenericTriangle<Vector3d<int32_t> > Triangle32;
typedef GenericTriangle<Vector3d<double> > Triangle3dd;
typedef GenericTriangle<Vector2d<double> > Triangle2dd;


class PointPath : public vector<Vector2dd>
{
public:
    PointPath(){}

    PointPath(int len) : vector<Vector2dd>(len)
    {}

    friend std::ostream & operator <<(std::ostream &out, const PointPath &pointPath)
    {
        out << "[";
        for (size_t i = 0; i < pointPath.size(); i++)
           out << (i == 0 ? "" : ", ") << pointPath.at(i) << std::endl;
        out << "]";
        return out;
    }

    /* This function checks if the poligon is inside the buffer. It assumes that the poligon coorinate can be rounded to upper value  */
    bool isInsideBuffer(const Vector2d<int> &bufferSize)
    {
        for (Vector2dd point : *this)
        {
            if (point.x() < 0 || point.y() < 0)
                return false;
            if (point.x() + 1 > bufferSize.x() || point.y() + 1 > bufferSize.y())
                return false;

        }
        return true;
    }
};

/**
 *  Polygon
 **/
class Polygon : public PointPath
{
public:
    Polygon(){}

    Polygon(const Vector2dd *points, int len) : PointPath(len)
    {
        for (unsigned i = 0; i < size(); i++) {
           this->operator[](i) = points[i];
        }
    }

    int  isInside(const Vector2dd &point) const;
    bool isConvex(bool *direction = NULL) const;

    Vector2dd getPoint (int i) const
    {
        return operator [](i);
    }

    Vector2dd getNormal(int i) const
    {
        Vector2dd r1 = operator []( i              );
        Vector2dd r2 = operator []((i + 1) % size());

        return (r2 - r1).rightNormal();
    }

    static Polygon FromRectagle(const Rectangled &rect) {
        Polygon toReturn;
        toReturn.reserve(4);
        toReturn.push_back(rect.ulCorner());
        toReturn.push_back(rect.urCorner());
        toReturn.push_back(rect.lrCorner());
        toReturn.push_back(rect.llCorner());
        return toReturn;
    }

    Polygon transform(const Matrix33 &transform) {
        Polygon toReturn;
        toReturn.reserve(size());
        for (Vector2dd p: *this ) {
            toReturn.push_back(transform * p);
        }
        return toReturn;

    }

    /* non const versions */
    double &x(int idx) {
        return operator [](idx).x();
    }

    double &y(int idx) {
        return operator [](idx).y();
    }

    /* const versions*/
    const double &x(int idx) const {
        return operator [](idx).x();
    }

    const double &y(int idx) const {
        return operator [](idx).y();
    }



    //bool clipRay(const Ray2d &ray, double &t1, double &t2);

};

} //namespace corecvs
#endif /* POLYGONS_H_ */

