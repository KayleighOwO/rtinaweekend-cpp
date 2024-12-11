#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "rtweekend.h"

class camera
{
    public:
        double aspect_ratio = 1.0;
        int image_width = 1920;
        int samples_per_pixel = 10;

        void render(const hittable& world)
        {
            initialise();
            
            image_file << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            for (int pixel_y = 0; pixel_y < image_height; pixel_y++)
            {
                std::clog << "\rScanlines remaining: " << (image_height - pixel_y) << ' ' << std::flush;
                for (int pixel_x = 0; pixel_x < image_width; pixel_x++)
                {
                    colour pixel_colour(0, 0, 0);
                    for (int sample = 0; sample < samples_per_pixel; sample++)
                    {
                        ray ray_obj = get_ray(pixel_x, pixel_y);
                        pixel_colour += ray_colour(ray_obj, world);
                    }
                    write_colour(image_file, pixel_samples_scale * pixel_colour);
                }
            }

            std::clog << "\rDone                 \n";
            image_file.close();
        }

    private:
        int image_height;
        double pixel_samples_scale;
        point3 camera_center;
        point3 pixel00_location;
        vec3 pixel_delta_x;
        vec3 pixel_delta_y;
        std::ofstream image_file;

        void initialise()
        {
            image_file.open("RTimg.ppm");
            
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_scale = 1.0 / samples_per_pixel;

            camera_center = point3(0, 0, 0);

            auto focal_length = 1.0;
            auto viewport_height = 2.0;
            auto viewport_width = viewport_height * (double(image_width) / image_height);
            
            camera_center = point3(0, 0, 0);

            // Calculate the vectors across the horizontal and down the vertical viewport edges
            auto viewport_vector_x = vec3(viewport_width, 0, 0);
            auto viewport_vector_y = vec3(0, -viewport_height, 0);

            // Calculate the horizontal and vertical delta vectors from pixel to pixel
            pixel_delta_x = viewport_vector_x / image_width;
            pixel_delta_y = viewport_vector_y / image_height;

            // Calculate the location of the upper left pixel.
            auto viewport_upper_left = camera_center
                                    - vec3(0, 0, focal_length)
                                    - viewport_vector_x / 2
                                    - viewport_vector_y / 2;
            pixel00_location = viewport_upper_left + 0.5 * (pixel_delta_x + pixel_delta_y);
        }

        ray get_ray(int pixel_x, int pixel_y) const
        {
            // Construct a camera ray originating from the origin and directed 
            // at a randomly sampled point around the pixel location x, y

            auto offset = camera_center;
            auto pixel_sample = pixel00_location
                              + ((pixel_x + offset.get_x()) * pixel_delta_x)
                              + ((pixel_y + offset.get_y()) * pixel_delta_y);

            auto ray_origin = camera_center;
            auto ray_direction = pixel_sample - ray_origin;

            return ray(ray_origin, ray_direction);
        }

        vec3 sample_square() const
        {
            // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square
            return vec3(random_double() - 0.5, random_double() - 0.5, 0);
        }
        
        colour ray_colour(const ray& ray_obj, const hittable& world)
        {
            hit_record record;
            if (world.hit(ray_obj, interval(0, infinity), record))
            {
                vec3 direction = random_on_hemisphere(record.surface_normal);
                return 0.5 * ray_colour(ray(record.intersection_point, direction), world);
            }
            
            vec3 unit_direction = unit_vector(ray_obj.get_direction());
            auto a = 0.5 * (unit_direction.get_y() + 1.0);
            return (1.0 - a) * colour(1.0, 1.0, 1.0)
                + a * colour(0.5, 0.7, 1.0);
        }
};

#endif