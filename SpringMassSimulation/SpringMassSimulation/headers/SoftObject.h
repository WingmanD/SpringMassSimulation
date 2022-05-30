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
    std::vector<Particle> flatParticles{};
    
public:
    SoftObject(Mesh* const mesh, Shader* const shader, std::vector<Force*>* environmentForces)
        : Object(mesh, shader, environmentForces) {
        setupParticles();
        initParticleBuffer();
    }

    void draw(Camera* camera, Transform instanceTransform, bool selected) override;

    void tick(double deltaTime) override;

    void appliedTranslate(glm::vec3 newLocation);
    

    ~SoftObject() override {
        Object::~Object();
        
        glDeleteVertexArrays(1, &VAO_particles);
        glDeleteBuffers(1, &VBO_particles);
        
        delete[] particles.data();
    }

private:
    void setupParticles();
    void initParticleBuffer();
    void drawParticles(Camera* camera) const;

};
