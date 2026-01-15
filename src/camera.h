#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "rtweekend.h"
#include "material.h"

class camera
{
    public:
        double aspect_ratio = 1.0;
        int image_width = 100;
        int samples_per_pixel = 10;
        int max_depth = 10;

        double vfov = 90;
        point3 look_from = point3(0, 0, 0);
        point3 look_at = point3(0, 0, -1);
        vec3 view_up = vec3(0, 1, 0); // Camera-relative up direction

        double defocus_angle = 0;   // Variation angle of rays through each pixel
        double focus_dist = 10;     // Distance from camera lookfrom point to plane of perfect focus

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
                        ray ray_obj = get_ray(pixel_y, pixel_x);
                        pixel_colour += ray_colour(ray_obj, max_depth, world);
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
        vec3 x, y, w;
        vec3 defocus_disk_x;
        vec3 defocus_disk_y;
        
        std::ofstream image_file;

        void initialise()
        {
            image_file.open("RTimg.ppm");
            
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_scale = 1.0 / samples_per_pixel;

            camera_center = look_from;

            //auto focal_length = (look_from - look_at).get_length();
            auto theta = degrees_to_radians(vfov);
            auto h = std::tan(theta / 2);
            auto viewport_height = 2 * h * focus_dist;
            //auto viewport_height = 2 * h * focal_length;
            auto viewport_width = viewport_height * (double(image_width) / image_height);
            
            // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
            w = unit_vector(look_from - look_at);
            x = unit_vector(cross(view_up, w));
            y = cross(w, x);
            
            // Calculate the vectors across the horizontal and down the vertical viewport edges.
            vec3 viewport_vector_x = viewport_width * x;
            vec3 viewport_vector_y = viewport_height * -y;

            // Calculate the horizontal and vertical delta vectors from pixel to pixel
            pixel_delta_x = viewport_vector_x / image_width;
            pixel_delta_y = viewport_vector_y / image_height;

            // auto viewport_upper_left = camera_center
            //                          - focal_length * w
            //                          - viewport_vector_x / 2
            //                          - viewport_vector_y / 2;
            auto viewport_upper_left = camera_center
                                     - focus_dist * w
                                     - viewport_vector_x / 2
                                     - viewport_vector_y / 2;
            pixel00_location = viewport_upper_left + 0.5 * (pixel_delta_x + pixel_delta_y);

            // Calculate the camera defocus disk basis vectors.
            auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_x = x * defocus_radius;
            defocus_disk_y = y * defocus_radius;
        }

        ray get_ray(int pixel_y, int pixel_x) const
        {
            // Construct a camera ray originating from the defocus disk and directed 
            // at a randomly sampled point around the pixel location x, y

            auto offset = sample_square();
            auto pixel_sample = pixel00_location
                              + ((pixel_y + offset.get_x()) * pixel_delta_y)
                              + ((pixel_x + offset.get_y()) * pixel_delta_x);

            //auto ray_origin = camera_center;
            auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;
            auto ray_time = random_double();

            // return ray(ray_origin, ray_direction);
            return ray(ray_origin, ray_direction, ray_time);
        }

        vec3 sample_square() const
        {
            // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square
            return vec3(random_double() - 0.5, random_double() - 0.5, 0);
        }

        point3 defocus_disk_sample() const
        {
            // Returns a random point in the camera defocus disk.
            auto point = random_in_unit_disk();
            return camera_center + (point[0] * defocus_disk_x) + (point[1] * defocus_disk_y);
        }
        
        colour ray_colour(const ray& ray_obj, int depth, const hittable& world) const
        {
            // If we've exceeded the ray bounce limit, no more light is gathered
            if (depth <= 0)
            {
                return colour(0, 0, 0);
            }
            
            hit_record record;
            
            if (world.hit(ray_obj, interval(0.001, infinity), record))
            {
                ray scattered;
                colour attenuation;
                if (record.mat -> scatter(ray_obj, record, attenuation, scattered))
                {
                    return attenuation * ray_colour(scattered, depth - 1, world);
                }

                return colour(0, 0, 0);
            }
            
            vec3 unit_direction = unit_vector(ray_obj.get_direction());
            auto a = 0.5 * (unit_direction.get_y() + 1.0);
            return (1.0 - a) * colour(1.0, 1.0, 1.0)
                + a * colour(0.5, 0.7, 1.0);
        }
};

#endif