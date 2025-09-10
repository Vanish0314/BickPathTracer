#include "Bounds.h"

bool Bounds::Intersect(const Ray& ray, double t_min, double t_max) const
{
    Vector3 t1 = (min - ray.origin) / ray.direction;
    Vector3 t2 = (max - ray.origin) / ray.direction;

    Vector3 tmin = Vector3::Min(t1, t2);
    Vector3 tmax = Vector3::Max(t1, t2);

    double near = std::max(tmin.x,std::max(tmin.y, tmin.z));
    double far  = std::min(tmax.x,std::min(tmax.y, tmax.z));

    if(near <= t_min && far >= t_max) return false;

    return std::max(near,t_min) <= std::min(far,t_max);
}