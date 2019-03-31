// author: ExecutionHY


#ifndef LIGHTH
#define LIGHTH

#include <vector>

#define MAX_TRIANGLE_NUM 1000

class Scene: public hitable  {
    public:
        Scene() {}
        void addGroup(Group *g) const;

        virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const;
        virtual float pdf_value(const vec3& o, const vec3& v) const;
        virtual vec3 random(const vec3& o) const;

        mutable std::vector<Group*> group_list; 

        mutable aabb bbox;
};

void Scene::addGroup(Group *g) const {
    group_list.push_back(g);
}

float Scene::pdf_value(const vec3& o, const vec3& v) const {
    int gcnt = group_list.size();
    float sum = 0;
    for (int i = 0; i < gcnt; i++)
        sum += group_list[i]->pdf_value(o, v);
    return sum / gcnt;
}

vec3 Scene::random(const vec3& o) const {
        int index = int(drand48() * group_list.size());
        return group_list[ index ]->random(o);
}


bool Scene::bounding_box(float t0, float t1, aabb& box) const {
    int gcnt = group_list.size();
    if (gcnt < 1) return false;
    aabb temp_box;
    bool first_true = group_list[0]->bounding_box(t0, t1, temp_box);
    if (!first_true)
        return false;
    else
        box = temp_box;
    for (int i = 1; i < gcnt; i++) {
        if(group_list[0]->bounding_box(t0, t1, temp_box)) {
            box = surrounding_box(box, temp_box);
        }
        else
            return false;
    }
    return true;
}

bool Scene::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    //std::cout << "? "<< std::endl;

    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;
    int gcnt = group_list.size();
    //std::cout << "?? " << gcnt << std::endl;
    for (int i = 0; i < gcnt; i++) {
        if (group_list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;

        }
    }
    return hit_anything;
}

#endif
