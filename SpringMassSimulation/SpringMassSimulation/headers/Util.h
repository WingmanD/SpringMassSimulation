#pragma once
#include <iostream>
#include <random>

#include "Shader.h"
#include <windows.h>
#include <shobjidl.h>
#include <glm/geometric.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Object.h"
#include "SoftObject.h"

class Util {
public:
    static Shader* loadShader(std::string path, std::string naziv) {
        std::string pathVert;
        std::string pathFrag;

        pathVert.append(path.substr(0, path.find_last_of("\\") + 1));
        pathFrag.append(path.substr(0, path.find_last_of("\\") + 1));


        if (pathFrag[pathFrag.size() - 1] == '/') {
            pathVert.append("shaders/");
            pathFrag.append("shaders/");
        }
        else if (pathFrag[pathFrag.size() - 1] == '\\') {
            pathVert.append("shaders\\");
            pathFrag.append("shaders\\");
        }
        else {
            std::cerr << "nepoznat format pozicije shadera" << std::endl;
            exit(1);
        }

        pathVert.append(naziv);
        pathVert.append(".vert");
        pathFrag.append(naziv);
        pathFrag.append(".frag");

        return new Shader(pathVert.c_str(), pathFrag.c_str());
    }

    static std::string WINAPI chooseOBJ() {
        std::string out;

        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
                                    COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr)) {
            IFileOpenDialog* pFileOpen;

            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                                  IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

            if (SUCCEEDED(hr)) {
                hr = pFileOpen->Show(NULL);

                if (SUCCEEDED(hr)) {
                    IShellItem* pItem;
                    hr = pFileOpen->GetResult(&pItem);
                    if (SUCCEEDED(hr)) {
                        PWSTR pszFilePath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                        if (SUCCEEDED(hr)) {
                            std::wstring ws(pszFilePath);
                            out = std::string(ws.begin(), ws.end());

                            CoTaskMemFree(pszFilePath);
                        }

                        pItem->Release();
                    }
                }
                pFileOpen->Release();
            }
            CoUninitialize();
        }

        return out;
    }

    static bool isVoxelOverlappingObject(Object* obj, glm::vec3 voxelCenter, float voxelSize) {
        const glm::vec3 voxelPivot = voxelCenter - glm::vec3(voxelSize * sqrt(3.0f) / 2.0f);
        for (int i = 0; i < 8; i++) {
            glm::vec3 voxelPoint = voxelPivot + glm::vec3(voxelSize * (i % 2), voxelSize * (i / 2) / 2.0f,
                                                          voxelSize * (i / 2) / 2.0f);
            if (isPointInsideObject(obj, voxelPoint)) { return true; }
        }

        glm::vec3 voxelMax = voxelCenter + glm::vec3(voxelSize * sqrt(3.0f) / 2.0f);

        for (auto i = 0; i < obj->mesh->getMesh()->mNumFaces; i++) {
            aiFace face = obj->mesh->getMesh()->mFaces[i];
            for (auto j = 0; j < face.mNumIndices; j++)
                if (isPointInsideObject(obj, aiVector3DToVec3(obj->mesh->getMesh()->mVertices[face.mIndices[j]])))
                    return true;
        }

        return false;
    }

    static bool isPointInsideObject(Object* obj, glm::vec3 point) {
        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution(0.0, 1.0);

        glm::vec3 rayDirection = glm::vec3(distribution(generator), distribution(generator), distribution(generator)) -
            point;
        //rayDirection = glm::vec3(1, 0, 0);
        rayDirection = glm::normalize(rayDirection);

        //std::cout << "mesh has face count: " << obj->mesh->getMesh()->mNumFaces << std::endl;

        int count = 0;
        for (int i = 0; i < obj->mesh->getMesh()->mNumFaces; i++) {
            RayTriangleIntersectionResult result = rayTriangleIntersection(
                point, rayDirection, obj->mesh->getMesh(), obj->mesh->getMesh()->mFaces[i]);
            if (result.intersected) count++;
        }

        //todo check for 90 deg ray and normal, check if ray hit a vertex - use enum inside struct,
        //check angle between normal and ray and check if intersection is equal to any of the vertices
        //std::cout << "intersection count: " << count << std::endl;
        return count % 2 != 0;
    }

    struct RayTriangleIntersectionResult {
        bool intersected = false;
        glm::vec3 intersectionPoint = glm::vec3(0);
    };

    static RayTriangleIntersectionResult rayTriangleIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                                                                 aiMesh* mesh,
                                                                 aiFace triangle) {

        RayTriangleIntersectionResult result;

        glm::vec3 v0 = aiVector3DToVec3(mesh->mVertices[triangle.mIndices[0]]);
        glm::vec3 v1 = aiVector3DToVec3(mesh->mVertices[triangle.mIndices[1]]);
        glm::vec3 v2 = aiVector3DToVec3(mesh->mVertices[triangle.mIndices[2]]);

        glm::vec3 e0 = v1 - v0;
        glm::vec3 e1 = v2 - v0;


        glm::vec3 n = glm::cross(e0, e1);
        float D = -glm::dot(n, v0);


        float denominator = glm::dot(n, rayDirection);
        if (denominator == 0.0f) return result;

        float t = -(glm::dot(n, rayOrigin) + D) / denominator;
        if (t < 0) return result;
        glm::vec3 intersection = rayOrigin + t * rayDirection;


        /*std::cout << "RTI for face: " << std::endl;
        std::cout << v0.x << " " << v0.y << " " << v0.z << std::endl;
        std::cout << v1.x << " " << v1.y << " " << v1.z << std::endl;
        std::cout << v2.x << " " << v2.y << " " << v2.z << std::endl;
        std::cout << "RTI n: " << n.x << " " << n.y << " " << n.z << std::endl;
        std::cout << "RTI D: " << D << std::endl;
        std::cout << "intersection: " << intersection.x << " " << intersection.y << " " << intersection.z << std::endl;
        std::cout << "denominator: " << glm::dot(n, rayDirection) << std::endl;
        std::cout << "t: " << t << std::endl;
        std::cout << "nominator: " << glm::dot(n, rayOrigin) + D << std::endl;
        std::cout << "origin: " << rayOrigin.x << " " << rayOrigin.y << " " << rayOrigin.z << std::endl;
        std::cout << "direction: " << rayDirection.x << " " << rayDirection.y << " " << rayDirection.z << std::endl;*/


        glm::vec3 p = intersection - v0;

        float t1 = glm::length(glm::cross(v2 - v0, p - v0)) / 2.0f;
        float t2 = glm::length(glm::cross(v0 - v1, p - v1)) / 2.0f;
        float t3 = glm::length(glm::cross(v1 - v2, p - v2)) / 2.0f;

        /*std::cout << "t1: " << t1 << std::endl;
        std::cout << "t2: " << t2 << std::endl;
        std::cout << "t3: " << t3 << std::endl;*/

        if (t1 >= 0 && t2 >= 0 && t3 >= 0 && t1 <= 1 && t2 <= 1 && t3 <= 1) {
            //std::cout << "intersection inside triangle" << std::endl;
            result.intersected = true;
            result.intersectionPoint = intersection;
        }

        return result;

    }

    static glm::vec3 aiVector3DToVec3(aiVector3D vec) { return glm::vec3(vec.x, vec.y, vec.z); }

};
