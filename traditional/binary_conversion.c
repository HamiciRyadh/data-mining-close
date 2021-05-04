//
// Created by hamici on 22/07/2019.
//

#include "binary_conversion.h"


void createBitmap(char filename[]) {
    double deb, fin;
    FILE *dataset = fopen(filename, "r");
    if (dataset == NULL) {
        printf("An error occurred.\n");
        exit(1);
    }
    char temp[500];
    IntList *intList = NULL;
    int rows = 0, variables = 0, length = 0, cols = 0;
    uint8_t **bitmap = NULL;

    //Read 1st line containing number of rows and variables
    fscanf(dataset, "%[^\n]\n", temp);
    intList = split(temp, SPLIT_BY, &length);
    rows = intList->value;
    intList = intList->next;
    variables = intList->value;

    printf("Starting to read dataset, rows : %d\n", rows);
    bitmap = (uint8_t**)malloc(rows* sizeof(uint8_t*));
    BitmapPosition position;

    cols = findBitmapPosition(variables).cell+1;
    deb = clock();
    for (int i = 0; i < rows; i++) {
        bitmap[i] = (uint8_t*)calloc(cols, sizeof(uint8_t));
//        for (int j = 0; j < findBitmapPosition(variables).cell+1; j++) printf("%d ", bitmap[i][j]);
//        printf("\n");
        fscanf(dataset, "%[^\n]\n", temp);
        intList = split(temp, SPLIT_BY, &length);
        while (intList != NULL) {
            position = findBitmapPosition(intList->value);
//            printf("Number %d, Cell %d, bit %d\n", intList->value, position.cell, position.bit);
            bitmap[i][position.cell] = bitmap[i][position.cell] | position.bit;

            intList = intList->next;
        }

//        for (int j = 0; j < findBitmapPosition(variables).cell+1; j++) printf("%d ", bitmap[i][j]);
//        printf("\n");
    }
    fin = clock();
    printf("End of dataset.\n");
    printf("Execution time : %lfs.\n", (fin - deb)/CLOCKS_PER_SEC);
    fclose(dataset);

    printf("Starting to create associated binary file.\n");
    FILE *binaryFile = fopen("binary.dat", "wb");
    if (binaryFile == NULL) {
        printf("An error occurred while creating binary file.\n");
        exit(1);
    }

    fwrite(&rows, sizeof(int), 1, binaryFile);
    fwrite(&cols, sizeof(int), 1, binaryFile);
    fwrite(&variables, sizeof(int), 1, binaryFile);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fwrite(&bitmap[i][j], sizeof(uint8_t), 1, binaryFile);
        }
    }
    fclose(binaryFile);
    printf("Binary file created.\n");
}

Matrix loadBitmap() {
    Matrix bitmap;
    FILE *binaryFile = fopen("binary.dat", "rb");
    if (binaryFile == NULL) {
        printf("An error occurred while creating binary file.\n");
        exit(1);
    }

    fread(&bitmap.rows, sizeof(int), 1, binaryFile);
    fread(&bitmap.cols, sizeof(int), 1, binaryFile);
    fread(&bitmap.variables, sizeof(int), 1, binaryFile);
    printf("Starting to read the binary file : Rows %d, cols %d\n", bitmap.rows, bitmap.cols);
    bitmap.bitmap = (uint8_t**)malloc(bitmap.rows* sizeof(uint8_t*));
    for (int i = 0; i < bitmap.rows; i++) {
        bitmap.bitmap[i] = (uint8_t*)malloc(bitmap.cols* sizeof(uint8_t));
        for (int j = 0; j < bitmap.cols; j++) {
            fread(&bitmap.bitmap[i][j], sizeof(uint8_t), 1, binaryFile);
        }
    }
    fclose(binaryFile);
    printf("Binary file read.\n");
    return bitmap;
}

BitmapPosition findBitmapPosition(int index) {
    BitmapPosition position;
    position.cell = (index-1)/8;
    switch ((index-1)%8) {
        case 0: {
            position.bit = BIT_1;
            break;
        }
        case 1: {
            position.bit = BIT_2;
            break;
        }
        case 2: {
            position.bit = BIT_3;
            break;
        }
        case 3: {
            position.bit = BIT_4;
            break;
        }
        case 4: {
            position.bit = BIT_5;
            break;
        }
        case 5: {
            position.bit = BIT_6;
            break;
        }
        case 6: {
            position.bit = BIT_7;
            break;
        }
        case 7: {
            position.bit = BIT_8;
            break;
        }
        default: {
            position.bit = BIT_1;
        }
    }
    return position;
}

IntList *split(char input[], char c, int *length) {
    IntList *list = NULL, *p = NULL, *q = NULL;
    int i, k = 0, lastC = 0, cpt = 0;
    char *temp;
    for (i = 0; i < strlen(input); i++) {
        if (input[i] == c && input[i-1] != c) {
            p = malloc(sizeof(IntList));
            p->next = NULL;
            temp = malloc((i-lastC+1)*sizeof(char));

            for (k = 0; k < i-lastC; k++) {
                temp[k] = input[lastC+k];
            }
            temp[i-lastC] = '\0';
            lastC = i+1;

            p->value = atoi(temp);
            if (list == NULL) {
                list = p;
                q = p;
            } else {
                q->next = p;
                q = p;
            }
            cpt++;
        }
    }

    if (input[i-1] != c) {
        p = malloc(sizeof(IntList));
        p->next = NULL;
        temp = malloc((i-lastC+1)*sizeof(char));

        for (k = 0; k < i-lastC; k++) {
            temp[k] = input[lastC+k];
        }
        temp[i-lastC] = '\0';

        p->value = atoi(temp);
        if (list == NULL) {
            list = p;
        } else {
            q->next = p;
        }
        cpt++;
    }
    *length = cpt;

    return list;
}
