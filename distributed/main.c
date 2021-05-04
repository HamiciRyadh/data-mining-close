#include "binary_conversion.h"
#include "close.h"

int borneInf1, borneSup1, borneInf2, borneSup2, id = 0;
Matrix matrix;


int main(int argc, char *argv[]) {
    createBitmap("chess.dat");
    matrix = loadBitmap();

    float minsup = atof(argv[1]);
    int borneInf1 = atoi(argv[2]);
    int borneSup1 = atoi(argv[3]);
    int borneInf2 = atoi(argv[4]);
    int borneSup2 = atoi(argv[5]);
    int id = atoi(argv[6]);

    if (minsup == 0) minsup = 0.7;
    printf("Minsup : %f, bf1 : %d, bs1 : %d, bf2 : %d, bs2 : %d\n", minsup, borneInf1, borneSup1, borneInf2, borneSup2);
    int bounds[2][2] = {{borneInf1, borneSup1}, {borneInf2, borneSup2}};
    closeAlgorithm(matrix, minsup, bounds, id);
    return 0;
}