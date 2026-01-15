#ifndef SPHERE_H
#define SPHERE_H

#include "rtweekend.h"
#include "hittable.h"

class sphere : public hittable
{
    public:
        // Stationary Sphere
        sphere(const point3& static_sphere_center, double sphere_radius, shared_ptr<material> mat)
                : center(static_sphere_center, vec3(0, 0, 0)), radius(std::fmax(0, sphere_radius)), mat(mat)
                {
                    auto ray_vector = vec3(sphere_radius, sphere_radius, sphere_radius);
                    bbox = aabb(static_sphere_center - ray_vector, static_sphere_center + ray_vector);
                }

        // Moving Sphere
        sphere(const point3& sphere_center1, const point3& sphere_center2, double sphere_radius, shared_ptr<material> mat)
                : center(sphere_center1, sphere_center2 - sphere_center1), radius(std::fmax(0, sphere_radius)), mat(mat)
                {
                    auto ray_vector = vec3(sphere_radius, sphere_radius, sphere_radius);
                    aabb box1(center.get_point_at(0) - ray_vector, center.get_point_at(0) + ray_vector);
                    aabb box2(center.get_point_at(1) - ray_vector, center.get_point_at(1) + ray_vector);
                    bbox = aabb(box1, box2);
                }

        bool hit(const ray& ray_obj,
                 interval ray_interval,
                 hit_record& record) const override
        {
            point3 current_center = center.get_point_at(ray_obj.time());
            vec3 origin_to_center = current_center - ray_obj.get_origin();
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
            vec3 outward_normal = (record.intersection_point - current_center) / radius;
            record.set_face_normal(ray_obj, outward_normal);
            record.mat = mat;

            return true;
        }

        aabb bounding_box() const override
        {
            return bbox;
        }

    private:
        ray center;
        double radius;
        shared_ptr<material> mat;
        aabb bbox;
};

#endif