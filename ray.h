#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
    public:
        ray() {}

        ray(const point3& ray_origin, const vec3& ray_direction)
            : origin_point(ray_origin), direction_vector(ray_direction) {}

        const point3& get_origin() const  { return origin_point; }
        const vec3& get_direction() const { return direction_vector; }

        // Returns the point along the ray at parameter t
        point3 get_point_at(double t) const
        {
            return origin_point + t * direction_vector;
        }

    private:
        point3 origin_point;        // Starting point of the ray
        vec3 direction_vector;      // Direction the ray is pointing
};

#endif