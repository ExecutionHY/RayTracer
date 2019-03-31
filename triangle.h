// author: ExecutionHY


#ifndef TRIANGLEH
#define TRIANGLEH

#include "hitable.h"
#include "vec3.h"

static const float kEpsilon = 1e-6;

class Triangle: public hitable  {
    public:
        Triangle() {}
        Triangle(vec3 _v0, vec3 _v1, vec3 _v2, vec3 _vn, material *mat) : v0(_v0), v1(_v1), v2(_v2), vn(_vn), mp(mat) {
            vec3 small( fmin(fmin(v0.x(), v1.x()), v2.x()),
                        fmin(fmin(v0.y(), v1.y()), v2.y()),
                        fmin(fmin(v0.z(), v1.z()), v2.z()) );
            vec3 big  ( fmax(fmax(v0.x(), v1.x()), v2.x()),
                        fmax(fmax(v0.y(), v1.y()), v2.y()),
                        fmax(fmax(v0.z(), v1.z()), v2.z()) );
            if (big.x() - small.x() < kEpsilon) big.e[0] += kEpsilon;
            if (big.y() - small.y() < kEpsilon) big.e[1] += kEpsilon;
            if (big.z() - small.z() < kEpsilon) big.e[2] += kEpsilon;
            bbox = aabb(small, big);
        };
        Triangle(vec3 _v0, vec3 _v1, vec3 _v2, vec3 _vn, MyMtl *mat) : v0(_v0), v1(_v1), v2(_v2), vn(_vn), mm(mat) {
            vec3 small( fmin(fmin(v0.x(), v1.x()), v2.x()),
                        fmin(fmin(v0.y(), v1.y()), v2.y()),
                        fmin(fmin(v0.z(), v1.z()), v2.z()) );
            vec3 big  ( fmax(fmax(v0.x(), v1.x()), v2.x()),
                        fmax(fmax(v0.y(), v1.y()), v2.y()),
                        fmax(fmax(v0.z(), v1.z()), v2.z()) );
            if (big.x() - small.x() < kEpsilon) big.e[0] += kEpsilon;
            if (big.y() - small.y() < kEpsilon) big.e[1] += kEpsilon;
            if (big.z() - small.z() < kEpsilon) big.e[2] += kEpsilon;
            bbox = aabb(small, big);
        };
        virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const;
        material  *mp;
        MyMtl *mm;
        vec3 v0, v1, v2, vn;
        aabb bbox;
        //float x0, x1, y0, y1, k;
};

bool Triangle::bounding_box(float t0, float t1, aabb& box) const {
    box = bbox;
    return true;

    // vec3 small( fmin(fmin(v0.x(), v1.x()), v2.x()),
    //             fmin(fmin(v0.y(), v1.y()), v2.y()),
    //             fmin(fmin(v0.z(), v1.z()), v2.z()) );
    // vec3 big  ( fmax(fmax(v0.x(), v1.x()), v2.x()),
    //             fmax(fmax(v0.y(), v1.y()), v2.y()),
    //             fmax(fmax(v0.z(), v1.z()), v2.z()) );
    // if (big.x() - small.x() < kEpsilon) big.e[0] += kEpsilon;
    // if (big.y() - small.y() < kEpsilon) big.e[1] += kEpsilon;
    // if (big.z() - small.z() < kEpsilon) big.e[2] += kEpsilon;
    // box = aabb(small, big);
    // return true;
}

bool Triangle::hit(const ray& r, float t0, float t1, hit_record& rec) const {
    
    //return false;
    // 1. Use bounding box to make a brief judgement about whether intersect or not
	if (!bbox.hit(r, t0, t1))
		return false;

    //v0.print(" ## ");

	// 2. Then calculate the possible intersect position
	vec3 v0v1 = v1 - v0;
	vec3 v0v2 = v2 - v0;
	vec3 pvec = cross(r.direction(), v0v2);
	float det =  dot(v0v1, pvec);

	// ray and triangle are parallel if det is close to 0
	if (fabs(det) < kEpsilon) return false;

	float invDet = 1 / det;

	vec3 tvec = r.origin() - v0;
	float u = dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1) return false;

	vec3 qvec = cross(tvec, v0v1);
	float v = dot(r.direction(), qvec) * invDet;
	if (v < 0 || u + v > 1) return false;

    rec.u = u;
    rec.v = v;
	//float t = dot(v0v2, qvec) * invDet;

    //vec3 N = unit_vector(cross(v0v1, v0v2)); // N 
    //float NdotRayDirection = dot(N, r.direction()); 
    //float d = dot(N, v0); 
    //float t = (dot(N, r.origin()) + d) / NdotRayDirection; 

    vec3 N = unit_vector(vn);
    vec3 ov0 = v0 - r.origin();
    float d = dot(N, ov0);
    float t = d / dot(N, r.direction());
    //std::cout << "t1 : " << t << ", " << d << " " << dot(N, r.direction()) << "\n";

    //t = (v0-r.origin()).length() / r.direction().length();
    //std::cout << "--t2 : " << t << ", " << (v0.z()-r.origin().z()) << " " << r.direction().z() << "\n";
    rec.t = t;
    rec.mat_ptr = mp;
    rec.mm_ptr = mm;
    rec.p = r.point_at_parameter(t);
    // Calculate face normal
    rec.normal = N;
    //rec.normal = vec3(0, 0, -1);
    return true;


    // // compute plane's normal
	// vec3 v0v1 = v1 - v0;
	// vec3 v0v2 = v2 - v0;
    // // no need to normalize
    // vec3 N = cross(v0v1, v0v2); // N 
    // float area2 = N.length(); 
 
    // // Step 1: finding P
 
    // // check if ray and plane are parallel ?
    // float NdotRayDirection = dot(N, r.direction()); 
    // if (fabs(NdotRayDirection) < kEpsilon) // almost 0 
    //     return false; // they are parallel so they don't intersect ! 
 
    // // compute d parameter using equation 2
    // float d = dot(N, v0); 
 
    // // compute t (equation 3)
    // float t = (dot(N, r.origin()) + d) / NdotRayDirection; 
    // // check if the triangle is in behind the ray
    // if (t < 0) return false; // the triangle is behind 
 
    // // compute the intersection point using equation 1
    // vec3 P = r.origin() + t * r.direction(); 
 
    // // Step 2: inside-outside test
    // vec3 C; // vector perpendicular to triangle's plane 
 
    // // edge 0
    // vec3 edge0 = v1 - v0; 
    // vec3 vp0 = P - v0; 
    // C = cross(edge0, vp0); 
    // if (dot(N, C) < 0) return false; // P is on the right side 
 
    // // edge 1
    // vec3 edge1 = v2 - v1; 
    // vec3 vp1 = P - v1; 
    // C = cross(edge1, vp1); 
    // if (dot(N, C) < 0)  return false; // P is on the right side 
 
    // // edge 2
    // vec3 edge2 = v0 - v2; 
    // vec3 vp2 = P - v2; 
    // C = cross(edge2, vp2); 
    // if (dot(N, C) < 0) return false; // P is on the right side; 
 
    // return true; // this ray hits the triangle 

}

#endif