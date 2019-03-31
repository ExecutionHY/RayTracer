// author: ExecutionHY


#ifndef LIGHTH
#define LIGHTH

#include <random>
#include "vec3.h"

#define MAX_TRIANGLE_NUM 1000

class Light {
public:
    vec3 le;
    float intensity;
    vec3 center;

    Light(const vec3 pos, const vec3 _le=vec3(1,1,1)) : center(pos), le(_le) {}
    virtual void illuminate(const vec3 &p, vec3 &lightDir, vec3 &le, float &dist) {}

};

class SphereLight : public Light {
public:
    float radius;

    SphereLight(const vec3 pos, const vec3 _le=vec3(1,1,1), 
        const float r=1.0f) : Light(pos, _le), radius(r) {}
    void illuminate(const vec3 &p, vec3 &lightDir, vec3 &le, float &dist) {
        lightDir = (p - center);
        float r2 = lightDir.length();
        dist = r2 - radius;
        lightDir = unit_vector(lightDir);
        le = this->le;
    }

};

class QuadLight : public Light {
public:
    vec3 v1, v2;
    float length, width, height;

    QuadLight(const vec3& centerPos, const vec3 c, vec3 _v1, vec3 _v2) : Light(centerPos, c)  {
        v1 = _v1;
        v2 = _v2;
        length = v2.x() - v1.x();
        height = v2.y() - v1.y();
        width = v2.z() - v1.z();
    }

    vec3 randomPt() {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution<float> distribution(0.0, 1.0);

		float r1 = distribution(gen);
		float r2 = distribution(gen);
		float r3 = distribution(gen);
		return vec3(v1.x() + r1*length, 
					v1.y() + r2*height, 
					v1.z() + r3*width);
    }

    void illuminate(const vec3 &p, vec3 &lightDir, vec3 &le, float &dist) {
        vec3 lightPos = randomPt();
        lightDir = (p - lightPos);
        dist = lightDir.length();
        lightDir = unit_vector(lightDir);
        le = this->le;
        //std::cout << "### " << le << std::endl;
    }

};


#endif
