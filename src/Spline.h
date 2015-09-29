//
//  Spline.h
//  Firetrail
//
//  Created by Etienne on 2015-09-29.
//
//

#ifndef Firetrail_Spline_h
#define Firetrail_Spline_h

class Spline
{
public:
    
    Spline()
    {
        points.resize(40);
        arcLengthLookup.resize(40);
        for (size_t i = 0; i < 40; ++i)
        {
            points.push_back(glm::vec3());
            arcLengthLookup.push_back(.0f);
        }
    }
    
    pushPoint(glm::vec3 pt)
    {
        const int npts = points.size();
        
        insertIndex = (insertIndex + 1) % npts; // move head
        
        if (writtenPoints < npts) ++writtenPoints;
        points[insertIndex] = pt;
        
        if (writtenPoints > 3)
        {
            // update arc length lookup table
            // pushing a point is like pushing p3 and lets you execute all calculations depending on p3
            arcLengthLookup[(insertIndex - 1 + npts) % npts] = computeArcLength(points[(insertIndex - 3 + npts) % npts],
                                                                                points[(insertIndex - 2 + npts) % npts],
                                                                                points[(insertIndex - 1 + npts) % npts],
                                                                                points[insertIndex], 50);
            
            // recompute curve length
            curveLength = .0f;
            int startIndex = insertIndex - writtenPoints + 3;
            for (size_t i = 0; i < writtenPoints - 3; ++i)
            {
                curveLength += arcLengthLookup[(startIndex + i) % npts];
            }
        }
    }
    
    glm::vec3 positionAtLength(float length)
    {
        if (writtenPoints < 4) return glm::vec3(.0f);
        
        const int npts = points.size();
        const int startIndex = insertIndex - writtenPoints + 3;
        float l = .0f;
        float arcLength = .0f;
        float index = 0;
        for (size_t i = 0; i < writtenPoints - 3; ++i)
        {
            arcLength = arcLengthLookup[index = (startIndex + i) % npts];
            if (l + arcLength > length) break;
            l += arcLength;
        }
        
        float t = (length - l) / arcLength;
        return interpolate(points[(index - 2 + npts) % npts],
                           points[(index - 1 + npts) % npts],
                           points[(index],
                           points[(index + 1 + npts) % npts], t);
    }
    
private:
    
    glm::vec3 interpolate(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
    {
        return 0.5 * ((p1 * 2) + (p2 - p0)*t + (p0 * 2 - p1 * 5 + p2 * 4 - p3) * t * t + \
                      (p1 * 3 + p3 - p0 - p2 * 3) * t * t * t);
    }
    
    float computeArcLength(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, size_t steps)
    {
        auto l = .0f;
        auto p = p1;
        for (size_t i = 0; i < steps; ++i)
        {
            var pn = interpolate(p0, p1, p2, p3, (float)i / (float)(steps - 1))
            l += glm::length(pn - p);
            p = pn;
        }
        return l;
    }
    
    std::vector<glm::vec3> points;
    std::vector<float> arcLengthLookup;
    int insertIndex{-1};
    int writtenPoints{0};
    float curveLength{.0f};
};

#endif
