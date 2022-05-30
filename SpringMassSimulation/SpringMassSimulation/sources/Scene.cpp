#include "Scene.h"

#include <iostream>

#include "SoftObject.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

void Scene::tick(double deltaTime) { for (auto& object : objects) { object->tick(deltaTime); } }

void Scene::load(std::string path, Shader* shader) {
    if (path.empty()) return;
    
    Assimp::Importer importer;

    std::cout << "Loading scene: " << path << std::endl;

    const aiScene* scene = importer.ReadFile(path.c_str(),
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType);

    if (!scene)
        std::cerr << importer.GetErrorString();

    for (int i = 0; i < scene->mNumMeshes; i++) {
        objects.emplace_back(new Object(new Mesh(scene->mMeshes[i]), shader, &environmentForces));
    }
}

void Scene::loadSoft(std::string path, Shader* shader) {
    if (path.empty()) return;
    
    Assimp::Importer importer;

    std::cout << "Loading soft scene: " << path << std::endl;

    const aiScene* scene = importer.ReadFile(path.c_str(),
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType);

    if (!scene)
        std::cerr << importer.GetErrorString();


    for (int i = 0; i < scene->mNumMeshes; i++)
        objects.emplace_back(new SoftObject(new Mesh(scene->mMeshes[i]), shader, &environmentForces));
}

void Scene::addObject(Object* object) { objects.emplace_back(object); }

void Scene::addForce(Force* force) { environmentForces.emplace_back(force); }

Scene::~Scene() {
    for (auto object : objects)
        delete object;
    for (auto force : environmentForces)
        delete force;
}
