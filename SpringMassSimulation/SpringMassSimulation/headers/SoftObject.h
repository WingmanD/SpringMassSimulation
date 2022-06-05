#pragma once
#include <iostream>
#include <random>
#include <vector>

#include "Mesh.h"
#include "Object.h"
#include "Particle.h"
#include "Shader.h"
#include "GLFW/glfw3.h"

class SoftObject : public Object {
    GLuint VAO_particles{};
    GLuint VBO_particles[2]{};

    std::vector<Particle*> particles;
    std::vector<Particle> flatParticles;

    std::vector<Object*>* otherObjectsInScene;

public:
    SoftObject(Mesh* const mesh, Material* material, Shader* const shader, std::vector<Object*>* objects,
               std::vector<Force*>* environmentForces)
        : Object(mesh, material, shader, environmentForces), otherObjectsInScene(objects) {

        setupParticles();
        initParticleBuffer();

        std::default_random_engine generator(glfwGetTime());
        const std::uniform_real_distribution<float> distribution(0.1, 1.0);
        material->setDiffuse({distribution(generator), distribution(generator), distribution(generator)});
    }

    void drawFirstPass(Camera* camera, Transform instanceTransform, std::vector<DirectionalLight*>& lights) override;
    void drawSecondPass(Camera* camera, Transform instanceTransform, std::vector<DirectionalLight*>& lights) override;

    void tick(double deltaTime) override;

    void appliedTranslate(glm::vec3 newLocation);

    ~SoftObject() override {
        Object::~Object();

        glDeleteVertexArrays(1, &VAO_particles);
        glDeleteBuffers(2, VBO_particles);

        delete[] particles.data();
    }

private:
    void setupParticles();
    void initParticleBuffer();
    void drawParticles(Camera* camera) const;
    void collide(Object* object) const;

};
