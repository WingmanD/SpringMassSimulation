#include "Shader.h"

#include <fstream>
#include <iostream>
#include <glad/glad.h>

void Shader::checkCompilerErrors(unsigned int shader, std::string type) {
    int success;
    char infolog[1024] = {};
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infolog);
            fprintf(
                stderr,
                "ERROR::SHADER_COMPILATION_ERROR of type: %d\n%s\n-----------------------------------------------------\n",
                type, infolog);
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infolog);
            fprintf(
                stderr,
                "ERROR::PROGRAM_LINKING_ERROR of type: %d\n%s\n-------------------------------------------------------\n",
                type, infolog);
        }
    }
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);


        std::ifstream test(geometryPath);
        if (test.fail()) geometryPath = nullptr;
        else gShaderFile.open(geometryPath);

        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        if (geometryPath) gShaderStream << gShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        if (geometryPath) gShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        if (geometryPath) geometryCode = gShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
        std::cout << geometryPath << std::endl;
    }
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment, geometry;
    int success;
    char infoLog[512];

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompilerErrors(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompilerErrors(fragment, "FRAGMENT");

    if (geometryPath) {
        // geometry Shader
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompilerErrors(geometry, "GEOMETRY");
    }

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryPath)
        glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompilerErrors(ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath)
        glDeleteShader(geometry);

}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::use() {

    glUseProgram(ID);
}

void Shader::setUniform(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setUniform(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);

}

void Shader::setUniform(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);

}
