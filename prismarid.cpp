#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;

#define HEIGHT 600
#define WIDTH 600
#define PRISM_HEIGHT 5
#define PRISM_RADIUS 5

// mesh center
float obj_x = 0;
float obj_y = 0;
float obj_z = 0;

// camera center
float cam_x = 0;
float cam_y = 0;
float cam_z = -20;

// light center
float lightx = 5;
float lighty = 0;
float lightz = -6;

// toggle state
int toggle = 0;

float angle = 0;
GLfloat up[3] = {0.0f, 1.0f, 0.0f};

void cross_product(float * v1, float * v2, float * cp){
    cp[0] = v1[1] * v2[2] - v2[1] * v1[2];
    cp[1] = v2[0] * v1[2] - v1[0] * v2[2];
    cp[2] = v1[0] * v2[1] - v2[0] * v1[1];
}

void normalize(float * v){
    float sum = 0;
    for(int i=0;i<3;i++){
        sum += v[i] * v[i];
    }
    float root = sqrt(sum);
    for(int i=0;i<3;i++){
        v[i] /= root;
    }
}

void process_input(GLFWwindow * window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
    }
    else if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
        obj_x -= 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
        obj_x += 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
        obj_y += 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS){
        obj_y -= 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS){
        obj_z -= 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        obj_z += 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        cam_x -= 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        cam_x += 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        cam_y += 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        cam_y -= 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        cam_z += 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cam_z -= 1;
    }
    else if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        angle += 5 * M_PI / 180;
    }
    else if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS){
        toggle ^= 1;
    }
    
}

void fb_size_cb(GLFWwindow * window, int width, int height){
    glViewport(0, 0, width, height);
}

const char * vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 pos;\n"
        "out vec4 fcolor;\n"
        "layout (location = 1) in vec3 normal;"
        "out vec3 frag;"
        "uniform mat4 transform;\n"
        "out vec3 normalf;"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "void main(){\n"
        "vec4 bfr_prj = view * transform * vec4(pos, 1.0);\n"
        "frag = vec3(transform * vec4(pos, 1.0)).xyz;"
        // "frag = normalize(out);"
        "gl_Position = vec4(bfr_prj.xyz, bfr_prj.z);"
        "normalf = normal;"
        "}\0";

const char * fragmentShaderSource = "#version 330 core\n"
        "uniform vec3 color;\n"
        "out vec4 FragColor;\n"
        "in vec4 fcolor;\n"
        "in vec3 frag;"
        "in vec3 normalf;"
        "uniform vec3 light_pos;"
        "void main(){\n"
        "float ambient_power = 0.3;"
        "vec3 light_direction = normalize(frag - light_pos);"
        "float val = dot(light_direction, normalf);"
        "float dot = max(val, 0.0f);"
        // "if(val > -0.5){ambient_power = 1;}"
        // "float dot = dot(light_direction, normalf);"
        "vec3 ambient_light = dot * vec3(1.0, 1.0, 1.0);"
        "vec3 out_color = ambient_light * color;"
        "FragColor = vec4(out_color, 1.0);\n"
        "}\0";

const char * light_shader = "#version 330 core\n"
        "layout (location = 0) in vec3 pos;\n"
        "out vec4 fcolor;\n"
        "uniform mat4 transform;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "void main(){\n"
        "vec4 bfr_prj = view * transform * vec4(pos, 1.0);\n"
        "gl_Position = vec4(bfr_prj.xyz, bfr_prj.z);"
        "}\0";

const char * light_fragment_shader = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec4 color;\n"
        "in vec4 fcolor;\n"
        "void main(){\n"
        "FragColor = color;\n"
        "}\0";


int main(int argc, char ** argv){
    if(argc != 2){
        cout << "Enter the number of faces\n";
        return -1;
    }
    int num_faces = atoi(argv[1]);
    if(num_faces == 2){
        cout << "Enter more than 2 faces\n";
        return -1;
    }
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "First OpenGL Program", NULL, NULL);
    if(!window){
        printf("create window error\n");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, fb_size_cb);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("glad initialization error\n");
        return 0;
    }
    
    Shader shader1("vsshader1.vs", "fsshader1.fs");

    unsigned int vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vertexShaderSource, NULL);
    glCompileShader(vertex_shader_id);
    int success;
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    printf("%d\n", success);

    unsigned int fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragment_shader_id);
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    printf("%d\n", success);

    unsigned int light_vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(light_vertex_shader_id, 1, &light_shader, NULL);
    glCompileShader(light_vertex_shader_id);
    glGetShaderiv(light_vertex_shader_id, GL_COMPILE_STATUS, &success);
    printf("%d\n", success);

    unsigned int light_fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(light_fragment_shader_id, 1, &light_fragment_shader, NULL);
    glCompileShader(light_fragment_shader_id);
    glGetShaderiv(light_fragment_shader_id, GL_COMPILE_STATUS, &success);
    printf("%d\n", success);

    unsigned int shader_prog_id = glCreateProgram();
    glAttachShader(shader_prog_id, vertex_shader_id);
    glAttachShader(shader_prog_id, fragment_shader_id);
    glLinkProgram(shader_prog_id);

    unsigned int light_shader_prog_id = glCreateProgram();
    glAttachShader(light_shader_prog_id, light_vertex_shader_id);
    glAttachShader(light_shader_prog_id, light_fragment_shader_id);
    glLinkProgram(light_shader_prog_id);

    // glViewport(0, 0, 800, 600);

    // define as if the center of the mesh was the origin
    // the mesh's side view will be seen
    // when you look with your eyes pointing to the origin and when sitting at the x axis
    int num_vertices = 2 * (num_faces + 1);
    int num_triangles = 4 * num_faces;
    int num_vertices2 = num_faces + 2;
    int num_triangles2 = 2 * num_faces;
    float vertices2[3 * num_vertices2];
    float vertices[3 * num_vertices];
    float theta = (360 * M_PI)/ (num_faces * 180);

    for(int i=0;i<num_vertices;i+=1){
        if(i == 0){
            vertices2[i*3] = vertices[i*3] = 0;
            vertices2[i*3 + 1] = vertices[i*3 + 1] = 0;
            vertices2[i*3 + 2] = vertices[i*3 + 2] = PRISM_HEIGHT/2;
        }
        else if(i == num_faces + 1){
            vertices2[i*3] = vertices[i*3] = 0;
            vertices2[i*3 + 1] = vertices[i*3 + 1] = 0;
            vertices2[i*3 + 2] = vertices[i*3 + 2] = -PRISM_HEIGHT/2;
        }
        else if(i < num_faces + 1){
            vertices2[i*3] = vertices[i*3] = sin((i - 1) * theta) * PRISM_RADIUS;
            vertices2[i*3 + 1] = vertices[i*3 + 1] = cos((i - 1) * theta) * PRISM_RADIUS;
            vertices2[i*3 + 2] = vertices[i*3 + 2] = PRISM_HEIGHT/2;
        }
        else{
            vertices[i*3] = sin((i - 2 - num_faces) * theta) * PRISM_RADIUS;
            vertices[i*3 + 1] = cos((i - 2 - num_faces) * theta) * PRISM_RADIUS;
            vertices[i*3 + 2] = -PRISM_HEIGHT/2;
        }
        // cout << vertices[i*3] << " " << vertices[i*3 + 1] << " " << vertices[i*3 + 2] << "\n";
    }

    unsigned int indices[num_triangles * 3];
    unsigned int indices2[num_triangles2 * 3];

    for(int i=0;i<num_triangles;i++){
        if(i < num_faces){
            indices2[i*3] = indices[i*3] = 0;
            indices2[i*3 + 1] = indices[i*3 + 1] = i + 1;
            if(i != num_faces - 1){
                indices2[i*3 + 2] = indices[i*3 + 2] = (i + 2);
            }
            else{
                indices2[i*3 + 2] = indices[i*3 + 2] = 1;
            }
        }
        else if(i < 3 * num_faces && i >= num_faces){
            if((i - num_faces)%2 == 0){
                indices[i*3] = (i - num_faces)/2 + 1;
                indices[i*3 + 1] = (i - num_faces)/2 + 1 + num_faces + 1;
                if((i - num_faces)/2 + 1 != num_faces){
                    indices[i*3 + 2] = (i - num_faces)/2 + 2;
                }
                else{
                    indices[i*3 + 2] = 1;
                }
            }
            else{
                indices[i*3] = (i - num_faces)/2 + num_faces + 1 + 1;
                if((i - num_faces)/2 + 2 != num_faces + 1){
                    indices[i*3 + 1] = (i - num_faces)/2 + 1 + num_faces + 1 + 1;
                    indices[i*3 + 2] = (i - num_faces)/2 + 2;
                }
                else{
                    indices[i*3 + 1] = num_faces + 2;
                    indices[i*3 + 2] = 1;
                }
            }
        }
        else{
            indices[i*3] = num_faces + 1;
            indices[i*3 + 1] = num_faces - (num_faces * 3) + i + 2;
            if(i != num_triangles - 1){
                indices[i*3 + 2] = (-2*num_faces + i + 3);
            }
            else{
                indices[i*3 + 2] = num_faces + 2;
            }
        }
    }

    for(int i=num_faces;i<num_triangles2;i++){
        indices2[i*3] = num_faces + 1;
        indices2[i*3 + 1] = (i - num_faces) + 1;
        if(i - num_faces + 1 != num_faces){
            indices2[i*3 + 2] = i - num_faces + 2;
        }
        else{
            indices2[i*3 + 2] = 1;
        }
    }

    // for a  only
    float normals[24] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
    };

    unsigned int VBO[4], VAO[3];

    // prism
    glGenVertexArrays(1, &VAO[0]);
    glBindVertexArray(VAO[0]);
    unsigned int EBO[3];
    glGenBuffers(1, &EBO[0]);

    glGenBuffers(1, &VBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glGenBuffers(1, &VBO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // light
    glGenVertexArrays(1, &VAO[2]);
    glBindVertexArray(VAO[2]);
    glGenBuffers(1, &EBO[2]);

    glGenBuffers(1, &VBO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // pyramid
    glGenVertexArrays(1, &VAO[1]);
    glBindVertexArray(VAO[1]);
    glGenBuffers(1, &EBO[1]);
    glGenBuffers(1, &VBO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);
    cout << shader_prog_id << "\n";
    while(!glfwWindowShouldClose(window)){
        
        float time = glfwGetTime()*20;
        time = 0;

        const GLfloat trans[] = {cos(angle), -sin(angle), 0.0f, obj_x,
                                sin(angle), cos(angle), 0.0f, obj_y,
                                0.0f, 0.0f, 1.0f, obj_z,
                                0.0f, 0.0f, 0.0f, 1.0f};

        const GLfloat trans2[] = {1.0, 0.0, 0.0f, lightx,
                                0.0, 1.0, 0.0f, lighty,
                                0.0f, 0.0f, 1.0f, lightz,
                                0.0f, 0.0f, 0.0f, 1.0f};

        GLfloat vecz[3] = {-cam_x + obj_x, -cam_y + obj_y, -cam_z + obj_z};
        GLfloat vecx[3], vecy[3];
        normalize(vecz);
        cross_product(vecz, up, vecx);
        normalize(vecx);
        cross_product(vecx, vecz, vecy);
        normalize(vecy);

        const GLfloat view[] = {vecx[0], vecx[1], vecx[2], -cam_x * vecx[0] - cam_y * vecx[1] - cam_z * vecx[2],
                                vecy[0], vecy[1], vecy[2],  -cam_x * vecy[0] - cam_y * vecy[1] - cam_z * vecy[2],
                                vecz[0], vecz[1], vecz[2],  -cam_x * vecz[0] - cam_y * vecz[1] - cam_z * vecz[2],
                                0.0f, 0.0f, 0.0f, 1.0f};

        const GLfloat projection[] = {1.0f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f};

        process_input(window);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // glUseProgram(3);
        shader1.use_program();

        int color = glGetUniformLocation(3, "color");
        glUniform3f(color, 0.5f, 0.3f, 0.2f);

        int t_loc = glGetUniformLocation(3, "transform");
        glUniformMatrix4fv(t_loc, 1, GL_TRUE, trans);

        int v_loc = glGetUniformLocation(3, "view");
        glUniformMatrix4fv(v_loc, 1, GL_TRUE, view);

        int p_loc = glGetUniformLocation(3, "projection");
        glUniformMatrix4fv(p_loc, 1, GL_TRUE, projection);

        int l_loc = glGetUniformLocation(3, "light_location");
        glUniform3f(l_loc, lightx, lighty, lightz);

        if(!toggle){
            glBindVertexArray(VAO[0]);
            glDrawElements(GL_TRIANGLES, num_triangles * 3, GL_UNSIGNED_INT, 0);
        }
        else{
            glBindVertexArray(VAO[1]);
            glDrawElements(GL_TRIANGLES, num_triangles2 * 3, GL_UNSIGNED_INT, 0);
        }

        glUseProgram(light_shader_prog_id);
        int lcolor = glGetUniformLocation(light_shader_prog_id, "color");

        int lt_loc = glGetUniformLocation(light_shader_prog_id, "transform");
        glUniformMatrix4fv(lt_loc, 1, GL_TRUE, trans2);

        int lv_loc = glGetUniformLocation(light_shader_prog_id, "view");
        glUniformMatrix4fv(lv_loc, 1, GL_TRUE, view);

        int lp_loc = glGetUniformLocation(light_shader_prog_id, "projection");
        glUniformMatrix4fv(lp_loc, 1, GL_TRUE, projection);
        glUniform4f(lcolor, 1.0f, 1.0f, 1.0f, 1.0f);

        glBindVertexArray(VAO[2]);
        glDrawElements(GL_TRIANGLES, num_triangles * 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}