#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class material
{
    public:
        virtual ~material() = default;

        virtual bool scatter(const ray& ray_in,
                             const hit_record& record,
                             colour& attenuation,
                             ray& scattered) const
            {
                return false;
            }
};

class lambertian : public material
{
    public:
        lambertian(const colour& albedo) : albedo(albedo) {}

        bool scatter(const ray& ray_in, 
                     const hit_record& record, 
                     colour& attenuation, 
                     ray& scattered)
        const override
        {
            auto scatter_direction = record.surface_normal + random_unit_vector();
            
            // Catch degenerate scatter direction
            if (scatter_direction.near_zero())
            {
                scatter_direction = record.surface_normal;
            }

            scattered = ray(record.intersection_point, scatter_direction);
            attenuation = albedo;
            
            return true;
        }

    private:
        colour albedo;
};

class metal : public material
{
    public:
        metal(const colour& albedo) : albedo(albedo) {}

        bool scatter(const ray& ray_in,
                     const hit_record& record,
                     colour& attenuation,
                     ray& scattered)
        const override
        {
            vec3 reflected = reflect(ray_in.get_direction(), record.surface_normal);
            scattered = ray(record.intersection_point, reflected);
            attenuation = albedo;

            return true;
        }

        private:
            colour albedo;
};

#endif