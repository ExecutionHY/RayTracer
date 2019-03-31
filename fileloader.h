// author: ExecutionHY


#ifndef OBJLOADERH
#define OBJLOADERH

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include "triangle.h"
#include "vec3.h"
#include "material.h"

#include "group.h"
#include "scene.h"

using namespace std;

class MTLloader {
public:
    mutable vector<MyMtl> mtlList;
    bool loadMTL(const string path) const;
    bool findMTL(const string mat, MyMtl **m_ptr) const;
    bool findMTL(const string mat, material **m_ptr) const;
};

bool MTLloader::loadMTL(const string path) const {
    
    ifstream fin(path);
    if (fin.fail()) {
        cout << "File not exist: " << path << endl;
        return false;
    }

    string type, name, tmp;
    fin >> type;
    while (true) {
        if (type == "newmtl") {
            MyMtl mtl;
            fin >> mtl.name;
            do {
                fin >> type;
                if (type == "illum") fin >> mtl.illum;
                else if (type == "Kd") fin >> mtl.kd.e[0] >> mtl.kd.e[1] >> mtl.kd.e[2];
                else if (type == "Ka") {
                    fin >> mtl.ka.e[0] >> mtl.ka.e[1] >> mtl.ka.e[2];
                    if (mtl.ka.length() > 0) mtl.self_luminous = true;
                }
                else if (type == "Ks") {
                    fin >> mtl.ks.e[0] >> mtl.ks.e[1] >> mtl.ks.e[2];
                    mtl.is_specular = true;
                }
                else if (type == "Tf") {
                    fin >> mtl.tf.e[0] >> mtl.tf.e[1] >> mtl.tf.e[2];
                    if (mtl.tf.x() < 1-kEpsilon || mtl.tf.y() < 1-kEpsilon || mtl.tf.z() < 1-kEpsilon)
                        mtl.is_transparent = true;
                }
                else if (type == "Ns") {
                    float tmp;
                    fin >> tmp;
                    mtl.ns = int(tmp);
                }
                else if (type == "Ni") {
                    fin >> mtl.ni;
                }
                else if (type == "newmtl") break;
            } while (fin.peek() != EOF);

            mtlList.push_back(mtl);
        }
        else break;
    }

    return true;
}
bool MTLloader::findMTL(const string mat, material **m_ptr) const {

    if (mat == "initialShadingGroup") {
        material *light = new diffuse_light( new constant_texture(vec3(40, 40, 40)) );
        *m_ptr = light;
        (*m_ptr)->ch = 'l';
        return true;
    }

    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    *m_ptr = white;
    (*m_ptr)->ch = 'w';
    return true;
}

bool MTLloader::findMTL(const string mat, MyMtl **m_ptr) const {

    for (int i = 0; i < mtlList.size(); i++) {
        if (mtlList[i].name == mat) {
            *m_ptr = &mtlList[i];
            return true;
        }
    }
    return false;
}

class OBJloader {
    public:
        OBJloader() {};
        bool gen_world(const string path, const string name, Scene & scene) const;
        MTLloader mtlLoader;
};

bool OBJloader::gen_world(const string path, const string name, Scene & scene) const {

    ifstream fin(path+name);
    if (fin.fail()) {
        cout << "File not exist: " << path+name << endl;
        return false;
    }

    //scene = new Scene();
    Group *currGroup = nullptr;
    MyMtl *currMat = nullptr;
    vector<vec3> verticles;
    vector<vec3> normals;
    bool smooth;
    stringstream ss;

    do {
        string type, tmp;
        fin >> type;
        if (type == "#") getline(fin, tmp);
        else if (type == "vt") getline(fin, tmp);
        else if (type == "mtllib") {
            string name;
            fin >> name;
            mtlLoader.loadMTL(path+name);
        }
        else if (type == "g") {
            currGroup = new Group();
            string name;
            fin >> name;
            if (name != "default") {
                scene.addGroup(currGroup);
                // material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
                // //white->ch = 'w';
                // currGroup->addTriangle(vec3(0, 0.4, 0.3), vec3(0.01, 0.4, 0.3), vec3(0, 0.4, 0.31), vec3(0,-1,0), white);
                // //std::cout << "--- " << currGroup->triangle_list[0]->mp->ch << std::endl;
            }
        }
        else if (type == "usemtl") {
            string matname;
            fin >> matname;
            if (!mtlLoader.findMTL(matname, &currMat)) {
                cout << "mtl not found: " << matname << endl;
                break;
            }
            cout << "add mat : " << currMat->name << endl;
        }
        else if (type == "s") {
            string mode;
            fin >> mode;
            if (mode == "off") smooth = false;
            else smooth = true;
        }
        else if (type == "v") {
            float x, y, z;
            fin >> x >> y >> z;
            verticles.push_back(vec3(x, y, z));
        }
        else if (type == "vn") {
            float x, y, z;
            fin >> x >> y >> z;
            normals.push_back(vec3(x, y, z));
        }
        else if (type == "f") {
            vector<int> face_v;
            vector<int> face_vn;
            getline(fin, tmp);
            //cout << "tmp: " << tmp << endl;
            ss.clear();
            ss << tmp;
            while (!ss.eof()) {
                string pt;
                ss >> pt;
                int pos0 = 0;
                int pos1 = pt.find_first_of("/");
                int pos2 = pt.find_last_of("/")+1;

                int v0 = stoi(pt.substr(pos0, pos1-pos0));
                int vn0 = stoi(pt.substr(pos2));

                face_v.push_back(v0-1);
                face_vn.push_back(vn0-1);
            }
            vec3 v0 = verticles[face_v[0]];
            vec3 vn0 = normals[face_vn[0]];
            for (int i = 0; i < face_v.size()-2; i++) {
                //cout << verticles[face_v[i+1]].x() << " " << verticles[face_v[i+1]].y() << " " << verticles[face_v[i+1]].z() << " " << endl;
                vec3 v1 = verticles[face_v[i+1]];
                vec3 vn1 = normals[face_vn[i+1]];
                vec3 v2 = verticles[face_v[i+2]];
                vec3 vn2 = normals[face_vn[i+2]];

                currGroup->addTriangle(v0, v1, v2, vn0, vn1, vn2, currMat);
                // aabb bbx;
                // currGroup->bounding_box(kEpsilon, MAXFLOAT, bbx);
                
                // bbx._min.print(" * ");
                // bbx._max.print(" + ");
                // cout << endl;
            }
        }

    } while (fin.peek() != EOF);


    for (int i = 0; i < scene.group_list.size(); i++) {
        Group *g = scene.group_list[i];
        g->buildBVH(kEpsilon, MAXFLOAT);
    }

    // material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );
    // light->ch = 'l';
    //currGroup->addTriangle(vec3(0, 0.4, 0.3), vec3(0.1, 0.4, 0.3), vec3(0, 0.4, 0.4), vec3(0,-1,0), light);
    //currGroup->addTriangle(vec3(0, 0, 0), vec3(0.3, 0.3, 0), vec3(0, 0.3, 0.3), vec3(-1,1,1), light);
    cout << "World generation success. group: " << scene.group_list.size() << endl;

    return true;
}


#endif