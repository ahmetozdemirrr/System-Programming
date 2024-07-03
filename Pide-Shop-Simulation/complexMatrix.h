#pragma once

/* complexMatrix.h */

#ifndef COMPLEX_MATRIX_H
#define COMPLEX_MATRIX_H

#define ROWS 3
#define COLS 4

typedef struct {
    double real;
    double imag;
} Complex;

void print_matrix(const char *name, Complex *matrix, int rows, int cols);
void conjugate_transpose(Complex *in, Complex *out, int rows, int cols);
void multiply_matrices(Complex *A, Complex *B, Complex *C, int rowsA, int colsA, int colsB);
void invert_matrix(Complex *in, Complex *out, int n);
void pseudo_inverse(Complex *A, Complex *pseudo_inv, int rows, int cols);

#endif /* COMPLEX_MATRIX_H */
