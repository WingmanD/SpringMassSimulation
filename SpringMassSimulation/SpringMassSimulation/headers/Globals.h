#pragma once

#include "Shader.h"

extern int width;
extern int height;

extern bool bWireframeMode;
extern bool bShowParticles;
extern bool bShowBoundingBoxes;

extern GLint projectionUniformLocation;
extern GLint viewUniformLocation;
extern GLint modelUniformLocation;

extern Shader* defaultShader;
extern Shader* particleShader;
