#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

#include <random>

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

        virtual bool scatter_thread_safe(const ray& ray_in,
                                         const hit_record& record,
                                         colour& attenuation,
                                         ray& scattered,
                                         std::mt19937& rng,
                                         std::uniform_real_distribution<double>& dist) const
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

            scattered = ray(record.intersection_point, scatter_direction, ray_in.time());
            attenuation = albedo;
            
            return true;
        }

        bool scatter_thread_safe(const ray& ray_in, 
                                 const hit_record& record, 
                                 colour& attenuation, 
                                 ray& scattered,
                                 std::mt19937& rng,
                                 std::uniform_real_distribution<double>& dist)
        const override
        {
            auto scatter_direction = record.surface_normal + random_unit_vector_thread_safe(rng, dist);
            
            // Catch degenerate scatter direction
            if (scatter_direction.near_zero())
            {
                scatter_direction = record.surface_normal;
            }

            scattered = ray(record.intersection_point, scatter_direction, ray_in.time());
            attenuation = albedo;
            
            return true;
        }

    private:
        colour albedo;

        vec3 random_unit_vector_thread_safe(std::mt19937& rng, std::uniform_real_distribution<double>& dist) const
        {
            while (true)
            {
                auto random_point = vec3(dist(rng) * 2.0 - 1.0, dist(rng) * 2.0 - 1.0, dist(rng) * 2.0 - 1.0);
                auto squared_length = random_point.get_length_squared();
                if (1e-160 < squared_length && squared_length <= 1)
                {
                    return random_point / sqrt(squared_length);
                }
            }
        }
};

class metal : public material
{
    public:
        metal(const colour& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

        bool scatter(const ray& ray_in,
                     const hit_record& record,
                     colour& attenuation,
                     ray& scattered)
        const override
        {
            vec3 reflected = reflect(ray_in.get_direction(), record.surface_normal);
            reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
            scattered = ray(record.intersection_point, reflected, ray_in.time());
            attenuation = albedo;

            return (dot(scattered.get_direction(), record.surface_normal) > 0);
        }

        bool scatter_thread_safe(const ray& ray_in,
                                 const hit_record& record,
                                 colour& attenuation,
                                 ray& scattered,
                                 std::mt19937& rng,
                                 std::uniform_real_distribution<double>& dist)
        const override
        {
            vec3 reflected = reflect(ray_in.get_direction(), record.surface_normal);
            reflected = unit_vector(reflected) + (fuzz * random_unit_vector_thread_safe(rng, dist));
            scattered = ray(record.intersection_point, reflected, ray_in.time());
            attenuation = albedo;

            return (dot(scattered.get_direction(), record.surface_normal) > 0);
        }

        private:
            colour albedo;
            double fuzz;

            vec3 random_unit_vector_thread_safe(std::mt19937& rng, std::uniform_real_distribution<double>& dist) const
            {
                while (true)
                {
                    auto random_point = vec3(dist(rng) * 2.0 - 1.0, dist(rng) * 2.0 - 1.0, dist(rng) * 2.0 - 1.0);
                    auto squared_length = random_point.get_length_squared();
                    if (1e-160 < squared_length && squared_length <= 1)
                    {
                        return random_point / sqrt(squared_length);
                    }
                }
            }
};

class dielectric : public material
{
    public:
        dielectric(double refraction_index) : refraction_index(refraction_index) {}

        bool scatter(const ray& ray_in, const hit_record& record, colour& attenuation, ray& scattered)
        const override
        {
            attenuation = colour(1.0, 1.0, 1.0);
            double ri = record.front_face ? (1.0 / refraction_index) : refraction_index;

            vec3 unit_direction = unit_vector(ray_in.get_direction());
            double cos_theta = std::fmin(dot(-unit_direction, record.surface_normal), 1.0);
            double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

            bool cannot_refract = ri * sin_theta > 1.0;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            {
                direction = reflect(unit_direction, record.surface_normal);
            }
            else
            {
                direction = refract(unit_direction, record.surface_normal, ri);
            }

            scattered = ray(record.intersection_point, direction, ray_in.time());
            return true;
        }

        bool scatter_thread_safe(const ray& ray_in, const hit_record& record, colour& attenuation, ray& scattered,
                                 std::mt19937& rng, std::uniform_real_distribution<double>& dist)
        const override
        {
            attenuation = colour(1.0, 1.0, 1.0);
            double ri = record.front_face ? (1.0 / refraction_index) : refraction_index;

            vec3 unit_direction = unit_vector(ray_in.get_direction());
            double cos_theta = std::fmin(dot(-unit_direction, record.surface_normal), 1.0);
            double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

            bool cannot_refract = ri * sin_theta > 1.0;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, ri) > dist(rng))
            {
                direction = reflect(unit_direction, record.surface_normal);
            }
            else
            {
                direction = refract(unit_direction, record.surface_normal, ri);
            }

            scattered = ray(record.intersection_point, direction, ray_in.time());
            return true;
        }
        
    private:
        // Refractive index in vacuum or air, or the ratio of the material's refractive index over
        // the refractive index of the enclosing media
        double refraction_index;

        static double reflectance(double cosine, double refraction_index)
        {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1 - refraction_index) / (1 + refraction_index);
            r0 = r0 * r0;
            return r0 + (1 - r0) * std::pow((1 - cosine), 5);
        }
};

#endif