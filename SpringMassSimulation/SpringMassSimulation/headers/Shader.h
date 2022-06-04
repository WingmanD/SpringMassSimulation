#pragma once

#include <string>
#include <sstream>

class Shader {
    void checkCompilerErrors(unsigned int shader, std::string type);

public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

    ~Shader();

    void use();
    void setUniform(const std::string& name, bool value) const;
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, float value) const;
};
