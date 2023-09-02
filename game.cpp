#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <stdlib.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <unistd.h>
using namespace std;

#include "shader.h"
#include "object.h"
#include "player.h"
#include "coin.h"
#include "laser.h"
#include "textobject.h"
#include "boundingbox.h"
#include "alarm.h"

#define HEIGHT 1000
#define WIDTH 1000

float player_dist = 0.0f;
float last_space_time = 0.0f;
float speed = 0.02;
float coin_size = 0.1;
float coin_gap = 0.05;
float player_size = 0.2;
float laser_height = 2.0f;
float player_y_speed = 0.03f;
float laser_width = 0.1f;
int score = 0;
int space_pressed = 0;
int level = 1;
float velocity = 0.0;
float level_dist[3] = {40.0, 50.0, 60.0};

float distance_left = 80.0;
float curr_dist = 0.0f;
float fpc = 0.05;
float alarm_motion = 0.0f;

int circle_point_intersection(float px, float py, float radius, float cx, float cy){
    float dist1 = (px - cx) * (px - cx) + (py - cy) * (py - cy);
    float dist2 = radius * radius;
    return dist1 <= dist2; 
}


int player_coin_collission(float x, float y){
    float player_x = -0.9;
    float player_y = player_dist - 1;
    float player_coords[] = {
        player_x, player_y,
        player_x, player_y + player_size,
        player_x + player_size, player_y,
        player_x + player_size, player_y + player_size
    };
    float laser_coords[] = {
        x, y,
        x + coin_size, y,
        x, y + coin_size,
        x + coin_size, y + coin_size
    };

    for(int i=0;i<4;i++){
        if(circle_point_intersection(laser_coords[i * 2], laser_coords[i*2 + 1], player_size/2, player_x, player_y + player_size/2)){
            return 1;
        }
    }

    return 0;
}


int player_laser_collision(LaserBox l){
    float player_x = -0.9;
    float player_y = player_dist - 1;

    float resolution = 0.001;
    int num_elems = (4.0/resolution) + 2;

    float laser_coords[num_elems * 2];
    for(int i=0;i<=2/resolution;i++){
        laser_coords[i * 2] = laser_width/2;
        laser_coords[i * 2 + 1] = -laser_height/2 + (resolution*i);
    }
    for(int i=2/resolution + 1;i < 4/resolution + 2;i++){
        laser_coords[i * 2] = -laser_width/2;
        laser_coords[i * 2 + 1] = -laser_height/2 + (resolution*(i - 2/resolution - 1));
    }

    float angle = l.angle;
    for(int i=0;i<num_elems/2;i++){
        float after_view_x = laser_coords[i*2];
        float after_view_y = laser_coords[i*2 + 1] * l.height/2.0;
        float after_rotation_x = cos(angle) * after_view_x - sin(angle) * after_view_y;
        float after_rotation_y = sin(angle) * after_view_x + cos(angle) * after_view_y;
        float trans_x = after_rotation_x + 2.0 + l.x - l.motion;
        float trans_y  = after_rotation_y + l.y + l.height/2;
        if(circle_point_intersection(trans_x, trans_y, player_size/2, player_x, player_y + player_size/2)){
            return 1;
        }
    }

    return 0;
}

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
    else if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        player_dist += 0.03f;
        last_space_time = glfwGetTime();
        velocity = player_y_speed;
        space_pressed = 1;
    }
    
}

void fb_size_cb(GLFWwindow * window, int width, int height){
    glViewport(0, 0, width, height);
}


int main(int argc, char ** argv){
    
    srand(time(NULL));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "Game Again", NULL, NULL);
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Object player = player_init();
    Object coin = coin_init();
    Object laser = laser_init();
    Object alarm = alarm_init();
    vector <CoinBox> v;
    CoinBox cb(0.05, 0.1);
    v.push_back(cb);
    TextObject to("fonts/COMIC.TTF");
    to.attatch_shader("text_vs.vs", "text_fs.fs");
    vector <LaserBox> lv;

    float threshold = 1.0f + cb.x + cb.width;
    float change = 0;


    while(!glfwWindowShouldClose(window)){

        // int level_change_cond = (curr_dist <= level_dist[level - 1] + fpc && curr_dist >= level_dist[level - 1] - fpc);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        cout << level << endl;

        if(player_dist > 2 - player_size){
            player_dist = 2 - player_size;
        }
        else if(player_dist > 0){
            player_dist += velocity - 2 * (glfwGetTime() - last_space_time) * 0.04;
            velocity -= 0.08 * (glfwGetTime() - last_space_time);
            if(velocity < 0){
                velocity = 0;
            }

        }
        else if(player_dist < 0){
            player_dist = 0;
        }

        if(level !=4 && (level == 0 || (curr_dist <= level_dist[level - 1] + fpc && curr_dist >= level_dist[level - 1] - fpc))){
            // change level
            if(level != 0){
                string next_level_text = "Level " + to_string(level) + " Passed";
                to.RenderText(next_level_text, -0.8, 0.0, 0.004);
            }
            if(level == 3){
                to.RenderText("You Win", -0.8, -0.6, 0.008);
                string score_text_win = "Your Final Score : " + to_string(score);
                to.RenderText(score_text_win, -0.9, -0.9, 0.003);
                glfwSwapBuffers(window);
                level++;
                sleep(1);
                continue;
            }
            if(level != 0){
                glfwSwapBuffers(window);
            }
            level++;
            distance_left = level_dist[level - 1];
            player_dist = 0;
            curr_dist = 0;
            change = 0;
            v.clear();
            lv.clear();
            CoinBox cb(0.05, 0.1);
            v.push_back(cb);
            threshold = 1.0f + cb.x + cb.width;
            speed += 0.01;
            sleep(1);
            continue;
        }

        string score_text = "Score : " + to_string(score);
        string level_text = "Level : " + to_string(level);
        string distance_left_text = "Distance Left : " + to_string(distance_left);
        to.RenderText(distance_left_text, 0.1, 0.93, 0.0015);
        to.RenderText(level_text, 0.0, -1, 0.002);
        to.RenderText(score_text, 0.5, -1, 0.002);

        GLfloat alarm_transform[] = {1.0f, 0.0f, 0.0f, 2.2 - alarm_motion,
                                0.0f, 1.0f, 0.0f, 1.8f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f};

        alarm.attatch_transform(alarm_transform);
        alarm.display(2);


        GLfloat transform[] = {1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, player_dist,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f};

        player.attatch_transform(transform);
        player.shader.use();
        int t_loc = glGetUniformLocation(player.shader.prog_id, "ycoord");
        glUniform1f(t_loc, player_dist - 1 + player_size / 2);
        if(space_pressed){
            space_pressed = 0;
            t_loc = glGetUniformLocation(player.shader.prog_id, "space");
            glUniform1f(t_loc, 1.0);
        }
        else{
            t_loc = glGetUniformLocation(player.shader.prog_id, "space");
            glUniform1f(t_loc, 0.0);
        }
        player.display(20);


        for(int k=0;k<v.size();k++){
            int horz = v[k].nhorz;
            int vert = v[k].nvert;
            CoinBox c = v[k];
            for(int i=0;i<vert;i++){
                for(int j=0;j<horz;j++){
                    float x_coord = 2.0 + (j + 1) * 0.05 + j * 0.1 + c.x - v[k].motion;
                    float y_coord = c.y + (i + 1) * 0.05 + i * 0.1;
                    int collision = player_coin_collission(x_coord, y_coord);
                    GLfloat transform2[] = {1.0f, 0.0f, 0.0f, 3.0 + (j + 1) * 0.05 + j * 0.1 + c.x - v[k].motion,
                                        0.0f, 1.0f, 0.0f, 1 + c.y + (i + 1) * 0.05 + i * 0.1,
                                        0.0f, 0.0f, 1.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f, 1.0f};
                    coin.attatch_transform(transform2);
                    if(collision && c.map[i][j]){
                        score++;
                    }
                    if(collision){
                        c.map[i][j] = 0;
                    }
                    if(c.map[i][j]){
                        coin.display(2);
                    }
                }
            }
            v[k].motion += speed;
            if(v[k].motion > 4.0f){
                v.erase(v.begin()+k);
                k--;
            }
        }

        for(int k=0;k<lv.size();k++){
            LaserBox l = lv[k];
            float angle = lv[k].angle;
            if(lv[k].rotate){
                lv[k].angle += 0.1f;
            }
            int collision = player_laser_collision(l);

            if(collision){
                to.RenderText("You Lose", -0.8, 0.0, 0.008);
                string score_text_lose = "Your Final Score : " + to_string(score);
                to.RenderText(score_text_lose, -0.8, -0.6, 0.003);
                level = 0;
                score = 0;
                break;
            }

            GLfloat transform2[] = {cos(angle), -sin(angle), 0.0f, 2.0 + lv[k].x - lv[k].motion,
                                    sin(angle), cos(angle), 0.0f, lv[k].y + lv[k].height/2,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f};

            GLfloat view[] = {1.0, 0.0f, 0.0f, 0.0f,
                            0.0f, lv[k].height/2.0, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f};
            float coeff[3];
            coeff[0] = sin(angle);
            coeff[1] = cos(angle);
            coeff[2] = -(lv[k].y + lv[k].height/2) * sin(angle) - (2.0 + lv[k].x - lv[k].motion) * cos(angle);
            laser.shader.use();
            t_loc = glGetUniformLocation(laser.shader.prog_id, "coeff");

            glUniform1fv(t_loc, 3, coeff);
            laser.attatch_view(view);
            laser.attatch_transform(transform2);
            laser.display(2);
            lv[k].motion += speed;
            if(lv[k].motion > 4.0f){
                lv.erase(lv.begin()+k);
                k--;
            }
        }

        if(change > threshold){
            change = 0;
            int choose = rand() % 2;

            if(choose){
                CoinBox newcoin(0.05, 0.1);
                v.push_back(newcoin);
                threshold = 1.0f + newcoin.x + newcoin.width;
            }
            else{
                LaserBox newlaser;
                lv.push_back(newlaser);
                threshold = 1.0f + newlaser.x + newlaser.width + newlaser.height * fabs(sin(newlaser.angle)) / 2;
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
        process_input(window);
        change += speed;
        curr_dist += speed;
        alarm_motion += speed;
        distance_left -= speed;
        if(distance_left < 0){
            distance_left = 0;
        }

        if(alarm_motion > 3.0){
            alarm_motion = 0;
        }

    }

}