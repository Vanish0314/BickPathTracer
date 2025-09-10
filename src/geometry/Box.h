// /*
//  * @Author: Vanish
//  * @Date: 2024-06-30 22:44:54
//  * @LastEditTime: 2024-07-13 05:49:22
//  * Also View: http://vanishing.cc
//  * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
//  */
// /*
//  * @Author: Vanish
//  * @Date: 2024-06-04 21:42:34
//  * @LastEditTime: 2024-07-13 05:44:18
//  * Also View: http://vanishing.cc
//  * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
//  */
// #pragma once

// #include "Hittable.h"

// class Box : public Hittable {
// public:
//     Vector3 m_BottomLeftCorner;
//     Vector3 m_TopRightCorner;
// private:
//     Quad m_Quad[6];

// public:
//     Box(std::string name, Vector3 min, Vector3 max, std::shared_ptr<Material> material):
//     Hittable(name,(min + max)/2,material), m_BottomLeftCorner(min), m_TopRightCorner(max) 
//     {
//         auto dx = Vector3(max.x - min.x, 0, 0);
//         auto dy = Vector3(0, max.y - min.y, 0);
//         auto dz = Vector3(0, 0, max.z - min.z);

//         m_Quad[0] = Quad("front" ,Vector3(min.x,min.y,max.z),dx,dy,material);
//         m_Quad[1] = Quad("right" ,Vector3(max.x,min.y,max.z),dy,dz,material);
//         m_Quad[2] = Quad("back"  ,Vector3(max.x,min.y,min.z),dx,dy,material);
//         m_Quad[3] = Quad("left"  ,Vector3(min.x,min.y,min.z),dy,dz,material);
//         m_Quad[4] = Quad("top"   ,Vector3(min.x,max.y,max.z),dx,dz,material);
//         m_Quad[5] = Quad("bottom",Vector3(min.x,min.y,min.z),dx,dy,material);
//     }
//     }
    

// public:
//     void Hit(const Ray& ray ,const Interval interval,HitRecord& hitRecord) override
//     {
        
//     }

// public:
//     void Rotate(const Vector3& elueAngle);
// };