#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "rtweekend.h"
#include "interval.h"

using colour = vec3;

inline double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
    {
        return std::sqrt(linear_component);
    }

    return 0;
}

void write_colour(std::ostream& out, const colour& pixel_colour)
{
    auto red_intensity = pixel_colour.get_x();
    auto green_intensity = pixel_colour.get_y();
    auto blue_intensity = pixel_colour.get_z();

    // Apply a linear to gamma transform for gamma 2
    red_intensity = linear_to_gamma(red_intensity);
    green_intensity = linear_to_gamma(green_intensity);
    blue_intensity = linear_to_gamma(blue_intensity);

    // Translate the [0,1] component values to the byte range [0,255].
    static const interval intensity(0.000, 0.999);
    int red_byte = int(256 * intensity.clamp(red_intensity));
    int green_byte = int(256 * intensity.clamp(green_intensity));
    int blue_byte = int(256 * intensity.clamp(blue_intensity));

    // Write out the pixel colour components.
    out << red_byte << ' ' << green_byte << ' ' << blue_byte << '\n';
}

#endif