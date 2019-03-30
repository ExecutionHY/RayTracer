// author: ExecutionHY


#ifndef GROUPH
#define GROUPH

#include "triangle.h"
#include "hitable_list.h"

#define MAX_TRIANGLE_NUM 1000

class Group: public hitable  {
    public:
        Group() { hitable **list = new hitable*[MAX_TRIANGLE_NUM]; idx = 0; }
        void addTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 vn, material *m) const;
        void finishGroup() const {
            list_ptr = new hitable_list(list, idx);
        }
        virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const {
               box =  bbox;
               return true; }
        hitable **list;
        mutable hitable *list_ptr;
        mutable int idx;
        mutable aabb bbox;
};

void Group::addTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 vn, material *m) const {
    
    hitable* tri = new Triangle(v0, v1, v2, vn, m);
    aabb bbox1;
    tri->bounding_box(0, 1, bbox1);
    bbox = surrounding_box(bbox, bbox1);
    list[idx++] = tri;

    //list_ptr = new hitable_list(list,6);
}


bool Group::hit(const ray& r, float t0, float t1, hit_record& rec) const {
    return list_ptr->hit(r, t0, t1, rec);
}

#endif
