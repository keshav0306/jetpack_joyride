#include "shader.h"
#include "object.h"
#include <iostream>
#include <glad/glad.h>

using namespace std;

Object::Object(){
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->ebo);
    glGenBuffers(1, &this->vbo[0]);
    glGenBuffers(1, &this->vbo[1]);
    glGenBuffers(1, &this->vbo[2]);
}

void Object::attatch_vertices(float * vertices, int num_elems){
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_elems, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

void Object::attatch_normals(float * normals, int num_elems){
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_elems, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
}

void Object::attatch_textures(float * texture, int num_elems){
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_elems, texture, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(2);
}

void Object::attatch_indices(unsigned int * indices, int num_elems){
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * num_elems, indices, GL_STATIC_DRAW);
}

void Object::attatch_transform(float * transform){
    this->shader.use();
    int t_loc = glGetUniformLocation(this->shader.prog_id, "transform");
    glUniformMatrix4fv(t_loc, 1, GL_TRUE, transform);
}

void Object::attatch_view(float * view){
    this->shader.use();
    int t_loc = glGetUniformLocation(this->shader.prog_id, "view");
    glUniformMatrix4fv(t_loc, 1, GL_TRUE, view);
}

void Object::attatch_projection(float * projection){
    this->shader.use();
    int t_loc = glGetUniformLocation(this->shader.prog_id, "projection");
    glUniformMatrix4fv(t_loc, 1, GL_TRUE, projection);
}

void Object::attatch_shader(char * vs_path, char * fs_path){
    this->shader.makeProgram(vs_path, fs_path);
}

void Object::display(int num_triangles){
    this->shader.use();
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, num_triangles * 3, GL_UNSIGNED_INT, 0);
}