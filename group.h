// author: ExecutionHY


#ifndef GROUPH
#define GROUPH

#include <vector>
#include "triangle.h"
#include "hitable_list.h"
#include "bvh.h"

class Group: public hitable  {
    public:
        Group() {}
        void addTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 vn, material *m) const;
        void addTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 vn, MyMtl *m) const;
        bool buildBVH(float t0, float t1) const {
            hitable **list = new hitable*[triangle_list.size()];
            for (int i = 0; i < triangle_list.size(); i++) {
                list[i] = triangle_list[i];
            }
            //*scene = new hitable_list(list,i);
            node = new bvh_node(list, triangle_list.size(), t0, t1);

            int tcnt = triangle_list.size();
            if (tcnt < 1) return false;
            aabb temp_box;
            bool first_true = triangle_list[0]->bounding_box(t0, t1, temp_box);
            if (!first_true)
                return false;
            else
                bbox = temp_box;
            for (int i = 1; i < tcnt; i++) {
                if(triangle_list[i]->bounding_box(t0, t1, temp_box)) {
                    bbox = surrounding_box(bbox, temp_box);
                }
                else
                    return false;
            }
            return true;

            //node = new bvh_node(triangle_list, t0, t1);
        }

        virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const;
        virtual float pdf_value(const vec3& o, const vec3& v) const;
        virtual vec3 random(const vec3& o) const;

        mutable std::vector<hitable*> triangle_list;
        //mutable hitable *list_ptr;
        mutable bvh_node *node;
        //mutable int idx;
        mutable aabb bbox;
};

void Group::addTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 vn, material *m) const {
    
    //hitable* tri = new Triangle(v0, v1, v2, vn, m);
    //aabb bbox1;
    //tri->bounding_box(0, 1, bbox1);
    //bbox = surrounding_box(bbox, bbox1);
    triangle_list.push_back(new Triangle(v0, v1, v2, vn, m));
    //triangle_list[triangle_list.size()-1]->v1.print("  ## ");
    //std::cout << "@@@ " << m->ch << " " << triangle_list[0]->mp->ch << std::endl;

    //list_ptr = new hitable_list(list,6);
}

void Group::addTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 vn, MyMtl *m) const {
    
    triangle_list.push_back(new Triangle(v0, v1, v2, vn, m));
}


float Group::pdf_value(const vec3& o, const vec3& v) const {
    int tcnt = triangle_list.size();
    float sum = 0;
    for (int i = 0; i < tcnt; i++)
        sum += triangle_list[i]->pdf_value(o, v);
    return sum / tcnt;
}

vec3 Group::random(const vec3& o) const {
        int index = int(drand48() * triangle_list.size());
        return triangle_list[ index ]->random(o);
}


bool Group::bounding_box(float t0, float t1, aabb& box) const {
    box = bbox;
    return true;

    // int tcnt = triangle_list.size();
    // if (tcnt < 1) return false;
    // aabb temp_box;
    // bool first_true = triangle_list[0]->bounding_box(t0, t1, temp_box);
    // if (!first_true)
    //     return false;
    // else
    //     box = temp_box;
    // for (int i = 1; i < tcnt; i++) {
    //     if(triangle_list[i]->bounding_box(t0, t1, temp_box)) {
    //         box = surrounding_box(box, temp_box);
    //     }
    //     else
    //         return false;
    // }
    // return true;
}

bool Group::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    //aabb bbx;
    //bounding_box(t_min, t_max, bbx);
    //bbx._max.print(" * ");
	if (!bbox.hit(r, t_min, t_max))
		return false;

    return node->hit(r, t_min, t_max, rec);

    //std::cout << "??? " << std::endl;
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;
    int tcnt = triangle_list.size();
    for (int i = 0; i < tcnt; i++) {
        if (triangle_list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

#endif
