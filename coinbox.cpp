#include <iostream>
#include "boundingbox.h"
using namespace std;

CoinBox::CoinBox(float gap_size, float coin_size){
    this->x = (float)rand() / (float)(RAND_MAX / 2) - 1;
    this->y = (float)rand() / (float)(RAND_MAX / 2) - 1;
    int max_x = (1 - this->x - gap_size )/ (coin_size + gap_size); 
    int max_y = (1 - this->y - gap_size )/ (coin_size + gap_size);

    if(max_x <= 0 || max_y <= 0){
        this->map = NULL;
        this->nvert = 0;
        this->nhorz = 0;
        this->width = 0;
        this->height = 0;
        goto end;
    }
    this->nvert = rand() % max_y;
    this->nhorz = rand() % max_x;
    this->width = (this->nhorz + 1) * gap_size + this->nhorz * coin_size;

    this->map = (int ** )malloc(sizeof(int *) * this->nvert);
    for(int i=0;i<this->nvert;i++){
        this->map[i] = (int *)malloc(sizeof(int) * this->nhorz);
    }
    for(int i=0;i<this->nvert;i++){
        for(int j=0;j<this->nhorz;j++){
            this->map[i][j] = 1;
        }
    }
    end:
    this->motion = 0.0f;
}