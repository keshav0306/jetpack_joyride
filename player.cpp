#include <iostream>
#include <math.h>
#include <glad/glad.h>
#include "shader.h"
#include "object.h"
#define RADIUS 0.10

Object player_init(){

    GLfloat projection[] = {1.0f, 0.0f, 0.0f, 0.0,
                        0.0f, 1.0f, 0.0f, 0.0,
                        0.0f, 0.0f, 1.0f, 0.0,
                        0.0f, 0.0f, 0.0f, 1.0f};


    GLfloat view[] = {1.0f, 0.0f, 0.0f, 0.0,
                            0.0f, 1.0f, 0.0f, 0.0,
                            0.0f, 0.0f, 1.0f, 0.0,
                            0.0f, 0.0f, 0.0f, 1.0f};
    
    int num_faces = 20;
    int num_vertices = (num_faces + 1);
    int num_triangles = num_faces;
    float theta = (360 * M_PI)/ (num_faces * 180);
    float vertices[3 * num_vertices];
    float center_x = -0.9;
    float center_y = -0.9;

    for(int i=0;i<num_vertices;i+=1){
        if(i == 0){
            vertices[i*3] = center_x;
            vertices[i*3 + 1] = center_y;
            vertices[i*3 + 2] = 0;
        }
        else{
            vertices[i*3] = sin((i - 1) * theta) * RADIUS + center_x;
            vertices[i*3 + 1] = cos((i - 1) * theta) * RADIUS + center_y;
            vertices[i*3 + 2] = 0;
        }
    }



    unsigned int indices[num_triangles * 3];
    for(int i=0;i<num_triangles;i++){
            indices[i*3] = 0;
            indices[i*3 + 1] = i + 1;
            if(i != num_faces - 1){
                indices[i*3 + 2] = (i + 2);
            }
            else{
                indices[i*3 + 2] = 1;
            }
    }


    // float vertices[] = {
    //     -0.9f, -1.0f, 0.0f,
    //     -0.8f, -1.0f, 0.0f,
    //     -0.9f, -0.9f, 0.0f,
    //     -0.8f, -0.9f, 0.0f
    // };

    // unsigned int indices[] = {
    //     0, 1, 2, 2, 3, 1
    // };

    Object obj;
    obj.attatch_vertices(vertices, num_vertices * 3);
    obj.attatch_indices(indices, num_triangles * 3);
    obj.attatch_shader("player_vs.vs", "player_fs.fs");
    obj.attatch_view(view);
    obj.attatch_projection(projection);
    return obj;
}