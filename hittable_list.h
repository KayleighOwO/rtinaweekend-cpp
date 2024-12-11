#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "rtweekend.h"
#include "hittable.h"

#include <vector>

class hittable_list : public hittable 
{
    public:
        std::vector<shared_ptr<hittable>> objects;

        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }

        void add(shared_ptr<hittable> object)
        {
            objects.push_back(object);
        }

        bool hit(const ray& ray_obj,
                 interval ray_interval,
                 hit_record& record) const override 
        {
            hit_record temp_record;
            bool hit_anything = false;
            auto closest_so_far = ray_interval.max;

            for (const auto& object : objects)
            {
                if (object->hit(ray_obj, interval(ray_interval.min, closest_so_far), temp_record))
                {
                    hit_anything = true;
                    closest_so_far = temp_record.t;
                    record = temp_record;
                }
            }

            return hit_anything;
    }
};

#endif