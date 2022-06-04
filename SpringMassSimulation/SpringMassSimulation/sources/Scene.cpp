#include "Scene.h"

#include <iostream>

#include "SoftObject.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

void Scene::tick(double deltaTime) { for (auto& object : objects) { object->tick(deltaTime); } }

Object* Scene::load(std::string path, Shader* shader, bool bLoadMaterials) {
    if (path.empty()) return nullptr;

    Assimp::Importer importer;

    std::cout << "Loading scene: " << path << std::endl;

    const aiScene* scene = importer.ReadFile(path.c_str(),
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType);

    if (!scene)
        std::cerr << importer.GetErrorString();

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        Material* material;
        if (scene->HasMaterials() && bLoadMaterials)
            material = new Material(scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]);
        else
            material = new Material();

        objects.emplace_back(new Object(new Mesh(scene->mMeshes[i]), material, shader, &environmentForces));
    }
    
    return objects.back();
}

Object* Scene::loadSoft(std::string path, Shader* shader, bool bLoadMaterials) {
    if (path.empty()) return nullptr;

    Assimp::Importer importer;

    std::cout << "Loading soft scene: " << path << std::endl;

    const aiScene* scene = importer.ReadFile(path.c_str(),
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType);

    if (!scene)
        std::cerr << importer.GetErrorString();


    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        Material* material;
        if (scene->HasMaterials() && bLoadMaterials)
            material = new Material(scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]);
        else
            material = new Material();

        auto newObject = new SoftObject(new Mesh(scene->mMeshes[i]), material, shader, &objects, &environmentForces);

        newObject->appliedTranslate(activeCamera->Location + activeCamera->getFront() * 10.0f);
        objects.emplace_back(newObject);
    }
    
    return objects.back();
}

void Scene::addObject(Object* object) { objects.emplace_back(object); }
void Scene::addLight(DirectionalLight* light) { lights.emplace_back(light); }

void Scene::addForce(Force* force) { environmentForces.emplace_back(force); }

Scene::~Scene() {
    for (auto object : objects)
        delete object;
    for (auto force : environmentForces)
        delete force;
}
