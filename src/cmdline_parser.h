#ifndef CMDLINE_PARSER_H
#define CMDLINE_PARSER_H

#include "rtweekend.h"
#include <iostream>
#include <string>

struct camera_config
{
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 512;
    int samples_per_pixel = 100;
    int max_depth = 100;
    double vfov = 20.0;
    point3 look_from = point3(13, 2, 3);
    point3 look_at = point3(0, 0, 0);
    vec3 view_up = vec3(0, 1, 0);
    double defocus_angle = 0.6;
    double focus_dist = 10.0;
};

void print_help(const char* program_name)
{
    std::cout << "Usage: " << program_name << " [OPTIONS]\n\n";
    std::cout << "Ray Tracer Camera Options:\n\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  --width WIDTH           Image width in pixels (default: 512)\n";
    std::cout << "  --aspect RATIO          Aspect ratio as decimal (default: 1.777778 for 16:9)\n";
    std::cout << "                          OR use --aspect W H for width:height ratio\n";
    std::cout << "  --samples SAMPLES       Samples per pixel (default: 100)\n";
    std::cout << "  --depth DEPTH           Maximum ray bounce depth (default: 100)\n";
    std::cout << "  --vfov ANGLE            Vertical field of view in degrees (default: 20)\n";
    std::cout << "  --lookfrom X Y Z        Camera position (default: 13 2 3)\n";
    std::cout << "  --lookat X Y Z          Point camera looks at (default: 0 0 0)\n";
    std::cout << "  --vup X Y Z             Camera up vector (default: 0 1 0)\n";
    std::cout << "  --defocus ANGLE         Defocus angle for depth of field (default: 0.6)\n";
    std::cout << "  --focusdist DIST        Focus distance (default: 10.0)\n\n";
    std::cout << "Example:\n";
    std::cout << "  " << program_name << " --width 1024 --samples 200 --lookfrom 10 3 5\n";
    std::cout << "  " << program_name << " --aspect 16 9 --width 1920\n";
    std::cout << "  " << program_name << " --aspect 2.35 --samples 500\n\n";
}

bool parse_vec3(int argc, char* argv[], int& i, vec3& v, const char* arg_name)
{
    if (i + 3 >= argc)
    {
        std::cerr << "Error: " << arg_name << " requires 3 values (X Y Z)\n";
        return false;
    }
    try
    {
        v = vec3(std::stod(argv[i+1]), std::stod(argv[i+2]), std::stod(argv[i+3]));
        i += 3;
        return true;
    }
    catch (...)
    {
        std::cerr << "Error: Invalid numeric values for " << arg_name << "\n";
        return false;
    }
}

bool parse_arguments(int argc, char* argv[], camera_config& config)
{
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--h" || arg == "-help" || arg == "--help")
        {
            print_help(argv[0]);
            return false;  // Return false to indicate program should exit (but not an error)
        }
        else if (arg == "--width")
        {
            if (i + 1 < argc)
            {
                try
                {
                    config.image_width = std::stoi(argv[++i]);
                    if (config.image_width <= 0)
                    {
                        std::cerr << "Error: Image width must be positive\n";
                        return false;
                    }
                }
                catch (...)
                {
                    std::cerr << "Error: Invalid value for --width\n";
                    return false;
                }
            }
            else
            {
                std::cerr << "Error: --width requires a value\n";
                return false;
            }
        }
        else if (arg == "--aspect")
        {
            if (i + 1 < argc)
            {
                try
                {
                    // Try to parse as single decimal value
                    config.aspect_ratio = std::stod(argv[i+1]);
                    if (config.aspect_ratio <= 0) {
                        std::cerr << "Error: Aspect ratio must be positive\n";
                        return false;
                    }
                    i++;
                }
                catch (...)
                {
                    // If first value fails, it might be the start of W H format
                    std::cerr << "Error: Invalid value for --aspect\n";
                    std::cerr << "Use: --aspect RATIO (e.g., 1.777) or --aspect W H (e.g., 16 9)\n";
                    return false;
                }
                
                // Check if there's another numeric value following (W H format)
                if (i + 1 < argc)
                {
                    try
                    {
                        // Try to parse as a number to see if user provided W H format
                        double height = std::stod(argv[i+1]);
                        // If we get here, user provided W H format
                        double width = config.aspect_ratio;  // First value was width
                        if (height <= 0) {
                            std::cerr << "Error: Aspect ratio height must be positive\n";
                            return false;
                        }
                        config.aspect_ratio = width / height;
                        i++;  // Consume the height value
                    }
                    catch (...)
                    {
                        // Next argument is not a number, so single decimal format was used
                        // This is fine, continue normally
                    }
                }
            }
            else
            {
                std::cerr << "Error: --aspect requires at least one value\n";
                std::cerr << "Use: --aspect RATIO (e.g., 1.777) or --aspect W H (e.g., 16 9)\n";
                return false;
            }
        }
        else if (arg == "--samples")
        {
            if (i + 1 < argc)
            {
                try
                {
                    config.samples_per_pixel = std::stoi(argv[++i]);
                    if (config.samples_per_pixel <= 0)
                    {
                        std::cerr << "Error: Samples per pixel must be positive\n";
                        return false;
                    }
                }
                catch (...)
                {
                    std::cerr << "Error: Invalid value for --samples\n";
                    return false;
                }
            }
            else
            {
                std::cerr << "Error: --samples requires a value\n";
                return false;
            }
        }
        else if (arg == "--depth")
        {
            if (i + 1 < argc)
            {
                try
                {
                    config.max_depth = std::stoi(argv[++i]);
                    if (config.max_depth <= 0)
                    {
                        std::cerr << "Error: Max depth must be positive\n";
                        return false;
                    }
                }
                catch (...)
                {
                    std::cerr << "Error: Invalid value for --depth\n";
                    return false;
                }
            }
            else
            {
                std::cerr << "Error: --depth requires a value\n";
                return false;
            }
        }
        else if (arg == "--vfov")
        {
            if (i + 1 < argc)
            {
                try
                {
                    config.vfov = std::stod(argv[++i]);
                    if (config.vfov <= 0 || config.vfov >= 180)
                    {
                        std::cerr << "Error: vfov must be between 0 and 180 degrees\n";
                        return false;
                    }
                }
                catch (...)
                {
                    std::cerr << "Error: Invalid value for --vfov\n";
                    return false;
                }
            }
            else
            {
                std::cerr << "Error: --vfov requires a value\n";
                return false;
            }
        }
        else if (arg == "--lookfrom")
        {
            if (!parse_vec3(argc, argv, i, config.look_from, "--lookfrom"))
            {
                return false;
            }
        }
        else if (arg == "--lookat")
        {
            if (!parse_vec3(argc, argv, i, config.look_at, "--lookat"))
            {
                return false;
            }
        }
        else if (arg == "--vup")
        {
            if (!parse_vec3(argc, argv, i, config.view_up, "--vup"))
            {
                return false;
            }
        }
        else if (arg == "--defocus")
        {
            if (i + 1 < argc)
            {
                try
                {
                    config.defocus_angle = std::stod(argv[++i]);
                    if (config.defocus_angle < 0)
                    {
                        std::cerr << "Error: Defocus angle must be non-negative\n";
                        return false;
                    }
                }
                catch (...)
                {
                    std::cerr << "Error: Invalid value for --defocus\n";
                    return false;
                }
            }
            else
            {
                std::cerr << "Error: --defocus requires a value\n";
                return false;
            }
        }
        else if (arg == "--focusdist")
        {
            if (i + 1 < argc)
            {
                try
                {
                    config.focus_dist = std::stod(argv[++i]);
                    if (config.focus_dist <= 0)
                    {
                        std::cerr << "Error: Focus distance must be positive\n";
                        return false;
                    }
                }
                catch (...)
                {
                    std::cerr << "Error: Invalid value for --focusdist\n";
                    return false;
                }
            }
            else
            {
                std::cerr << "Error: --focusdist requires a value\n";
                return false;
            }
        }
        else
        {
            std::cerr << "Error: Unrecognized argument '" << arg << "'\n\n";
            print_help(argv[0]);
            return false;
        }
    }
    
    return true;
}

#endif