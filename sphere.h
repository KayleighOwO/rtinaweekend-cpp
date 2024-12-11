#ifndef SPHERE_H
#define SPHERE_H

#include "rtweekend.h"
#include "hittable.h"

class sphere : public hittable
{
    public:
        sphere(const point3& sphere_center, double sphere_radius)
             : center(sphere_center), radius(std::fmax(0, sphere_radius)) {}

        bool hit(const ray& ray_obj,
                 interval ray_interval,
                 hit_record& record) const override
        {
            // Vector from ray origin to sphere center
            vec3 origin_to_center = center - ray_obj.get_origin();

            // Coefficients for the quadratic equation
            auto direction_length_squared = ray_obj.get_direction().get_length_squared();
            auto half_b = dot(ray_obj.get_direction(), origin_to_center);
            auto c = origin_to_center.get_length_squared() - (radius * radius);

            auto discriminant = (half_b * half_b) - (direction_length_squared * c);

            if (discriminant < 0)
            {
                return false;
            }

            double sqrt_discriminant = std::sqrt(discriminant);

            // Find the nearest intersection root within the valid range
            auto root = (half_b - sqrt_discriminant) / direction_length_squared;
            if (!ray_interval.surrounds(root))
            {
                root = (half_b + sqrt_discriminant) / direction_length_squared;
                if (!ray_interval.surrounds(root))
                {
                    return false;
                }
            }

            record.t = root;
            record.intersection_point = ray_obj.get_point_at(record.t);
            vec3 outward_normal = (record.intersection_point - center) / radius;
            record.set_face_normal(ray_obj, outward_normal);

            return true;
        }

    private:
        point3 center;
        double radius;
};

#endif