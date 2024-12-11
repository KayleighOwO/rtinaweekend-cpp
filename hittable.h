#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

class hit_record
{
    public:
        point3 intersection_point;
        vec3 surface_normal;
        double t;
        bool front_face;

        void set_face_normal(const ray& ray_obj, const vec3& outward_normal)
        {
            // Sets the hit record normal vector
            // The parameter `outward_normal` is assumed to have unit length

            front_face = dot(ray_obj.get_direction(), outward_normal) < 0;
            surface_normal = front_face ? outward_normal : -outward_normal;
        }
};

class hittable
{
    public:
        virtual ~hittable() = default;

        virtual bool hit(const ray& ray_obj,
                         interval ray_interval,
                         hit_record& record) const = 0;
};

#endif