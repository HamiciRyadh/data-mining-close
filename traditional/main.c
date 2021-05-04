#include "binary_conversion.h"
#include "close.h"

int main(void) {
    createBitmap("dataset.dat");
    Matrix matrix = loadBitmap();
    closeAlgorithm(matrix, 0.66);
    return 0;
}