#include "shader.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <glad/glad.h>
using namespace std;

Shader::Shader(){
    this->fs_id = 0;
    this->vs_id = 0;
    this->prog_id = 0;
}

void Shader::makeProgram(char * vsPath, char * fsPath){
    attatchVs(vsPath);
    attatchFs(fsPath);
    if(!this->vs_id || !this->fs_id){
        std::cout << "VS OR FS FAILED TO COMPILE\n";
    }
    this->prog_id = glCreateProgram();
    glAttachShader(this->prog_id, this->vs_id);
    glAttachShader(this->prog_id, this->fs_id);
    glLinkProgram(this->prog_id);
    int success;
    char infoLog[512];
    // print linking errors if any
    glGetProgramiv(this->prog_id, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(this->prog_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

Shader::Shader(char * vsPath, char * fsPath){
    makeProgram(vsPath, fsPath);
}

void Shader::attatchFs(char * fsPath){
    int fd = open(fsPath, O_RDONLY);
    char * mem =  (char * )calloc(1024, sizeof(char));
    read(fd, mem, 1024);
    const char * c = mem;
    this->fs_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->fs_id, 1, &c, NULL);
    glCompileShader(this->fs_id);
    int success;
    char infoLog[512];
    glGetShaderiv(this->fs_id, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(this->fs_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
}

void Shader::attatchVs(char * vsPath){
    int fd = open(vsPath, O_RDONLY);
    char * mem =  (char * )calloc(1024, sizeof(char));
    read(fd, mem, 1024);
    this->vs_id = glCreateShader(GL_VERTEX_SHADER);
    const char * c = mem;
    glShaderSource(this->vs_id, 1, &c, NULL);
    glCompileShader(this->vs_id);
    int success;
    char infoLog[512];
    glGetShaderiv(this->vs_id, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(this->vs_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
}

void Shader::use(){
    glUseProgram(this->prog_id);
}