#pragma once
#include <iostream>
#include <vector>

#include "Mesh.h"
#include "Object.h"
#include "Particle.h"
#include "Shader.h"

class SoftObject : public Object {
    GLuint VAO_particles = -1;
    GLuint VBO_particles = -1;
    std::vector<Particle*> particles{};
    
public:
    SoftObject(Mesh* const mesh, Shader* const shader, std::vector<Force*>* environmentForces)
        : Object(mesh, shader, environmentForces) { initParticles(); }

    void draw(Camera* camera, Transform instanceTransform, bool selected) override;

    void tick(double deltaTime) override;

    ~SoftObject() override {
        Object::~Object();
        glDeleteVertexArrays(1, &VAO_particles);
        glDeleteBuffers(1, &VBO_particles);
    }

private:
    void initParticles();
    void drawParticles(Camera* camera);

};
