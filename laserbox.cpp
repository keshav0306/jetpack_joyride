#include <iostream>
#include "boundingbox.h"
#include <math.h>
using namespace std;

LaserBox::LaserBox(){
    this->x = (float)rand() / (float)(RAND_MAX);
    this->y = (float)rand() / (float)(RAND_MAX / 2) - 1;
    this->rotate = rand() % 2;
    if(this->y == 0){
        goto end;
    }
    this->height = (float)rand() / (float)(RAND_MAX / (1 - this->y));
    assert(this->height < 1  - this->y);
    this->width = 0.1f;
    this->angle = (float)rand() / (float)(RAND_MAX / M_PI) - M_PI_2;
    end:
    this->motion = 0.0f;
}