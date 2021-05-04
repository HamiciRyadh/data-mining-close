//
// Created by hamici on 22/07/2019.
//

#ifndef DATA_MINING_CLOSE_BINARY_CONVERSION_H
#define DATA_MINING_CLOSE_BINARY_CONVERSION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define SPLIT_BY ' '
#define BIT_1 128
#define BIT_2 64
#define BIT_3 32
#define BIT_4 16
#define BIT_5 8
#define BIT_6 4
#define BIT_7 2
#define BIT_8 1

typedef struct IntList {
    int value;
    struct IntList *next;
} IntList;

typedef struct BitmapPosition {
    unsigned int cell;
    uint8_t bit;
} BitmapPosition;

typedef struct Matrix {
    int rows;
    int cols;
    int variables;
    uint8_t **bitmap;
} Matrix;

IntList *split(char input[], char c, int *length);
BitmapPosition findBitmapPosition(int index);
void createBitmap(char filename[]);
Matrix loadBitmap();


#endif //DATA_MINING_CLOSE_BINARY_CONVERSION_H
