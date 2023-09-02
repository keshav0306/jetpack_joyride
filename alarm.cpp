#include "iostream"
#include <glad/glad.h>
#include "shader.h"
#include "object.h"

Object alarm_init(){

    GLfloat projection[] = {1.0f, 0.0f, 0.0f, 0.0,
                        0.0f, 1.0f, 0.0f, 0.0,
                        0.0f, 0.0f, 1.0f, 0.0,
                        0.0f, 0.0f, 0.0f, 1.0f};


    GLfloat view[] = {1.0f, 0.0f, 0.0f, 0.0,
                            0.0f, 1.0f, 0.0f, 0.0,
                            0.0f, 0.0f, 1.0f, 0.0,
                            0.0f, 0.0f, 0.0f, 1.0f};

     float vertices[] = {
        -1.0f, -1.0f, 0.0f,
        -0.9f, -1.0f, 0.0f,
        -0.9f, -0.9f, 0.0f,
        -1.0f, -0.9f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0
    };

    Object obj;
    obj.attatch_vertices(vertices, 12);
    obj.attatch_indices(indices, 6);
    obj.attatch_shader("alarm_vs.vs", "alarm_fs.fs");
    obj.attatch_view(view);
    obj.attatch_projection(projection);
    return obj;
}