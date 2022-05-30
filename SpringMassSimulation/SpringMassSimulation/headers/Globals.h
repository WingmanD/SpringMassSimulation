#pragma once

#include "Shader.h"

extern int width;
extern int height;

extern bool bWireframeMode;
extern bool bShowParticles;
extern bool bShowBoundingBoxes;

extern Shader* defaultShader;
extern Shader* particleShader;

extern float particleMass;
extern float springConstant;
extern float internalSpringConstant;
extern float damping;
extern float internalPressureForceConstant;
