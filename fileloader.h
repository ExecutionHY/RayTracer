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
    vector<material> materialList;
    bool loadMTL(const string path) const;
    bool findMTL(const string mat, material *m) const;
};

bool MTLloader::loadMTL(const string path) const {
    
    return true;
}
bool MTLloader::findMTL(const string mat, material *m) const {

    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    m = white;
    return true;
}

class OBJloader {
    public:
        OBJloader() {};
        bool gen_world(const string path, const string name, hitable* world) const;
        MTLloader mtlLoader;
};

bool OBJloader::gen_world(const string path, const string name, hitable* world) const {

    ifstream fin(path+name);
    if (fin.fail()) {
        cout << "File not exist: " << path+name << endl;
        return false;
    }

    Group *currGroup = nullptr;
    material *currMat = nullptr;
    Scene scene;
    vector<vec3> verticles;
    vector<vec3> normals;

    do {
        string type, tmp;
        fin >> type;
        if (type == "#") getline(fin, tmp);
        else if (type == "vt") getline(fin, tmp);
        else if (type == "mtllib") {
            string name;
            fin >> name;
            mtlLoader.loadMTL(name);
        }
        else if (type == "g") {
            currGroup = new Group();
            string name;
            fin >> name;
            if (name == "default") {
                verticles.clear();
                normals.clear();
            }
            else {
                scene.addGroup(currGroup);
            }

        }
        else if (type == "usemtl") {
            string matname;
            fin >> matname;
            if (!mtlLoader.findMTL(matname, currMat)) {
                cout << "mtl not found: " << matname << endl;
                break;
            }
        }
        else if (type == "v") {

        }

    } while (!fin.eof());

    return true;
}


#endif