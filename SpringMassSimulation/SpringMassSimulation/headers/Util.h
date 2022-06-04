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
        std::string pathGeom;

        pathVert.append(path, 0, path.find_last_of("\\/") + 1);
        pathFrag.append(path, 0, path.find_last_of("\\/") + 1);
        pathGeom.append(path, 0, path.find_last_of("\\/") + 1);
        if (pathFrag[pathFrag.size() - 1] == '/') {
            pathVert.append("shaders/");
            pathFrag.append("shaders/");
            pathGeom.append("shaders/");
        }
        else if (pathFrag[pathFrag.size() - 1] == '\\') {
            pathVert.append("shaders\\");
            pathFrag.append("shaders\\");
            pathGeom.append("shaders\\");
        }
        else {
            std::cerr << "Shader " << pathVert << " not found!" << std::endl;
            exit(1);
        }

        pathVert.append(naziv);
        pathVert.append(".vert");
        pathFrag.append(naziv);
        pathFrag.append(".frag");
        pathGeom.append(naziv);
        pathGeom.append(".geom");

        return new Shader(pathVert.c_str(), pathFrag.c_str(), pathGeom.c_str());
    }

    static std::string WINAPI chooseOBJ() {
        std::string out;

        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED |
                                    COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr)) {
            IFileOpenDialog* pFileOpen;

            hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                                  IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

            if (SUCCEEDED(hr)) {
                hr = pFileOpen->Show(nullptr);

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

    static bool isPointInsideObject(Object* obj, glm::vec3 point) {
        std::default_random_engine generator(glfwGetTime());
        const std::uniform_real_distribution<float> distribution(0.0, 1.0);

        glm::vec3 rayDirection = glm::vec3(distribution(generator), distribution(generator), distribution(generator)) -
            point;
        
        rayDirection = normalize(rayDirection);
        
        int count = 0;
        for (const auto triangle : obj->mesh->triangles) {
            RayTriangleIntersectionResult result = rayTriangleIntersection(
                point, rayDirection, triangle);
            if (result.intersected) count++;
        }

        return count % 2 != 0;
    }

    struct RayTriangleIntersectionResult {
        bool intersected = false;
        glm::vec3 intersectionPoint = glm::vec3(0);
    };

    static RayTriangleIntersectionResult rayTriangleIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                                                                 Triangle triangle) {

        RayTriangleIntersectionResult result;

        glm::vec3 v0 = triangle.v0->position;
        glm::vec3 v1 = triangle.v1->position;
        glm::vec3 v2 = triangle.v2->position;

        glm::vec3 e0 = v1 - v0;
        glm::vec3 e1 = v2 - v0;

        // calculate plane
        glm::vec3 n = triangle.getNormal();
        float D = -dot(n, v0);

        // calculate intersection with plane
        float denominator = dot(n, rayDirection);
        if (denominator == 0.0f) return result;

        float t = -(dot(n, rayOrigin) + D) / denominator;
        if (t < 0) return result;
        glm::vec3 intersection = rayOrigin + t * rayDirection;
        
        // check if intersection is inside triangle
        e0 = triangle.v1->position - triangle.v0->position;
        glm::vec3 v0P = intersection - triangle.v0->position;
        glm::vec3 C = cross(e0, v0P);

        if (dot(n, C) < 0) return result;

        e1 = triangle.v2->position - triangle.v1->position;
        glm::vec3 v1P = intersection - triangle.v1->position;
        C = cross(e1, v1P);
        if (dot(n, C) < 0) return result;

        glm::vec3 e2 = triangle.v0->position - triangle.v2->position;

        glm::vec3 v2P = intersection - triangle.v2->position;
        C = cross(e2, v2P);
        if (dot(n, C) < 0) return result;

        result.intersected = true;
        result.intersectionPoint = intersection;
        
        return result;
    }

};
