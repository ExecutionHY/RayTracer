#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <sstream>

#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "hitable.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "aarect.h"

#include "triangle.h"
#include "fileloader.h"

using namespace std;

hitable* random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    texture* checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), 
        new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(checker));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();
            vec3 center(a+0.9*drand48(), 0.2, b+0.9*drand48());
            if ((center - vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) { // diffuse
                    list[i++] = new moving_sphere(center, center+vec3(0,0.5*drand48(),0), 
                        0.0, 1.0, 0.2,
                        new lambertian(new constant_texture(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48()))
                    ));
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2, new metal(
                        vec3(0.5*(1+drand48()), 0.5*(1+drand48()), 0.5*(1+drand48())),
                        0.5*drand48()
                    ));
                }
                else { // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }   
            }
        }
    }

    list[i++] = new sphere(vec3(0,1,0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4,1,0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    list[i++] = new sphere(vec3(4,1,0), 1.0, new metal(vec3(0.7,0.6,0.5), 0.0));

    return new hitable_list(list, i);
}


void cornell_box2(hitable **scene, camera **cam, float aspect) {
    int i = 0;
    hitable **list = new hitable*[10];
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    material *glass = new dielectric(1.5);
    list[i++] = new sphere(vec3(190, 90, 190),90 , glass);
    //list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 444, white));
    list[i++] = new Triangle(vec3(222, 222, 222), vec3(333, 333, 222), vec3(222, 333, 333), vec3(-1, 1, 1), white);
    *scene = new hitable_list(list,i);
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278,278,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
    *cam = new camera(lookfrom, lookat, vec3(0,1,0),
                      vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}

inline vec3 de_nan(const vec3& c) {
    vec3 temp = c;
    if (!(temp[0] == temp[0])) temp[0] = 0;
    if (!(temp[1] == temp[1])) temp[1] = 0;
    if (!(temp[2] == temp[2])) temp[2] = 0;
    return temp;
}

vec3 color(const ray& r, hitable *world, hitable *light_shape, int depth) {
    hit_record hrec;
    if (world->hit(r, 0.001, MAXFLOAT, hrec)) {
        scatter_record srec;
        vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);
        if (depth < 50 && hrec.mat_ptr->scatter(r, hrec, srec)) {
            if (srec.is_specular) {
                return srec.attenuation * color(srec.specular_ray, world, light_shape, depth+1);
            }
            else {
                hitable_pdf plight(light_shape, hrec.p);
                mixture_pdf p(&plight, srec.pdf_ptr);
                ray scattered = ray(hrec.p, p.generate(), r.time());
                float pdf_val = p.value(scattered.direction());
                //delete srec.pdf_ptr;
                return emitted + srec.attenuation*hrec.mat_ptr->scattering_pdf(r, hrec, scattered)*color(scattered, world, light_shape, depth+1) / pdf_val;
            }
        }
        else
            return emitted;
    }
    else
        return vec3(0,0,0);
}



int main() {
    int nx = 400;
    int ny = 400;
    int ns = 10;
    time_t now = time(0);
    stringstream ss;
    ss << now;
    string filename = "result"+ss.str()+".ppm";
    ofstream fout(filename);
    fout << "P3\n" << nx << " " << ny << "\n255\n";

    // hitable* list[5];
    // list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    // list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    // list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
    // list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
    // list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
    // hitable* world = new hitable_list(list, 5);
    //hitable* world = cornell_box();

    hitable *world;
    camera *cam;
    float aspect = float(ny) / float(nx);
    cornell_box2(&world, &cam, aspect);
    hitable *light_shape = new xz_rect(213, 343, 227, 332, 554, 0);
    hitable *glass_sphere = new sphere(vec3(190, 90, 190), 90, 0);
    hitable *a[2];
    a[0] = light_shape;
    a[1] = glass_sphere;
    hitable_list hlist(a,2);

    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;

    //camera cam(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);
    for (int j = ny-1; j >= 0; j--) {
        if (j % 10 == 0) cout << "line: " << j << endl;
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; s++) {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam->get_ray(u, v);
                //vec3 p = r.point_at_parameter(2.0);
                col += de_nan(color(r, world, &hlist, 0));
            }

            //vec3 p = r.point_at_parameter(2.0);
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            fout << ir << " " << ig << " " << ib << endl;
        }
    }
}