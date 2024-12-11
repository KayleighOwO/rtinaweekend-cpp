#ifndef VEC3_H
#define VEC3_H

#include "rtweekend.h"

class vec3
{
    public:
        double components[3]; // Holds x, y, z components

        vec3() : components{0, 0, 0} {}
        vec3(double x, double y, double z) : components{x, y, z} {}

        double get_x() const { return components[0]; }
        double get_y() const { return components[1]; }
        double get_z() const { return components[2]; }

        vec3 operator-() const { return vec3(-components[0], -components[1], -components[2]); }
        double operator[](int index) const { return components[index]; }
        double& operator[](int index) { return components[index]; }

        vec3& operator += (const vec3& vector)
        {
            components[0] += vector.components[0];
            components[1] += vector.components[1];
            components[2] += vector.components[2];
            return *this;
        }

        vec3& operator *= (double scalar)
        {
            components[0] *= scalar;
            components[1] *= scalar;
            components[2] *= scalar;
            return *this;
        }

        vec3& operator /= (double scalar)
        {
            return *this *= 1 / scalar;
        }

        double get_length() const
        {
            return std::sqrt(get_length_squared());
        }

        double get_length_squared() const
        {
            return components[0]*components[0] + components[1]*components[1] + components[2]*components[2];
        }

        static vec3 get_random()
        {
            return vec3(random_double(), random_double(), random_double());
        }

        static vec3 get_random(double min, double max)
        {
            return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
        }
};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using point3 = vec3;


// Vector Utility Functions

// Output a vector to a stream in the format "x y z"
inline std::ostream& operator<<(std::ostream& out, const vec3& vector)
{
    return out << vector.components[0] << ' ' << vector.components[1] << ' ' << vector.components[2];
}

// Add two vectors component-wise
inline vec3 operator+(const vec3& vector1, const vec3& vector2)
{
    return vec3(vector1.components[0] + vector2.components[0],
                vector1.components[1] + vector2.components[1],
                vector1.components[2] + vector2.components[2]);
}

// Subtract the second vector from the first component-wise
inline vec3 operator-(const vec3& vector1, const vec3& vector2)
{
    return vec3(vector1.components[0] - vector2.components[0],
                vector1.components[1] - vector2.components[1],
                vector1.components[2] - vector2.components[2]);
}

// Multiply two vectors component-wise
inline vec3 operator*(const vec3& vector1, const vec3& vector2)
{
    return vec3(vector1.components[0] * vector2.components[0],
                vector1.components[1] * vector2.components[1],
                vector1.components[2] * vector2.components[2]);
}

// Scale a vector by a scalar from the left
inline vec3 operator*(double scalar, const vec3& vector)
{
    return vec3(scalar * vector.components[0],
                scalar * vector.components[1],
                scalar * vector.components[2]);
}

// Scale a vector by a scalar from the right
inline vec3 operator*(const vec3& vector, double scalar)
{
    return scalar * vector;
}

// Divide a vector by a scalar
inline vec3 operator/(const vec3& vector, double scalar)
{
    return (1 / scalar) * vector;
}

// Compute the dot product of two vectors
inline double dot(const vec3& vector1, const vec3& vector2)
{
    return vector1.components[0] * vector2.components[0]
         + vector1.components[1] * vector2.components[1]
         + vector1.components[2] * vector2.components[2];
}

// Compute the cross product of two vectors
inline vec3 cross(const vec3& vector1, const vec3& vector2)
{
    return vec3(vector1.components[1] * vector2.components[2] - vector1.components[2] * vector2.components[1],
                vector1.components[2] * vector2.components[0] - vector1.components[0] * vector2.components[2],
                vector1.components[0] * vector2.components[1] - vector1.components[1] * vector2.components[0]);
}

// Compute the unit vector (normalize the input vector)
inline vec3 unit_vector(const vec3& vector)
{
    return vector / vector.get_length();
}

inline vec3 random_unit_vector()
{
    while (true)
    {
        auto random_point = vec3::get_random(-1, 1);
        auto squared_length = random_point.get_length_squared();
        if (1e-160 < squared_length && squared_length <= 1)
        {
            return random_point / sqrt(squared_length);
        }
    }
}

inline vec3 random_on_hemisphere(const vec3& normal)
{
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0)// In the same hemisphere as the normal
    {
        return on_unit_sphere;
    }
    else
    {
        return -on_unit_sphere;
    }
}

#endif