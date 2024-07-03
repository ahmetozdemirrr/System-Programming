/* complexMatrix.c */

#include "systemHelper.h"
#include "complexMatrix.h"

void print_matrix(const char *name, Complex *matrix, int rows, int cols) 
{
    printf("%s = [\n", name);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("(%lf, %lf) ", matrix[i * cols + j].real, matrix[i * cols + j].imag);
        }
        printf("\n");
    }
    printf("]\n");
}

void conjugate_transpose(Complex *in, Complex *out, int rows, int cols) 
{
    for (int i = 0; i < rows; i++) 
    {
        for (int j = 0; j < cols; j++) 
        {
            out[j * rows + i].real = in[i * cols + j].real;
            out[j * rows + i].imag = -in[i * cols + j].imag;
        }
    }
}

void multiply_matrices(Complex *A, Complex *B, Complex *C, int rowsA, int colsA, int colsB) 
{
    for (int i = 0; i < rowsA; i++) 
    {
        for (int j = 0; j < colsB; j++) 
        {
            C[i * colsB + j].real = 0;
            C[i * colsB + j].imag = 0;

            for (int k = 0; k < colsA; k++) 
            {
                C[i * colsB + j].real += A[i * colsA + k].real * B[k * colsB + j].real - A[i * colsA + k].imag * B[k * colsB + j].imag;
                C[i * colsB + j].imag += A[i * colsA + k].real * B[k * colsB + j].imag + A[i * colsA + k].imag * B[k * colsB + j].real;
            }
        }
    }
}

void invert_matrix(Complex *in, Complex *out, int n) 
{
    Complex **aug = malloc(n * sizeof(Complex *));
    for (int i = 0; i < n; i++) 
    {
        aug[i] = malloc(2 * n * sizeof(Complex));
    }

    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < n; j++) 
        {
            aug[i][j] = in[i * n + j];
            if (i == j) 
            {
                aug[i][j + n].real = 1.0;
                aug[i][j + n].imag = 0.0;
            } 
            else 
            {
                aug[i][j + n].real = 0.0;
                aug[i][j + n].imag = 0.0;
            }
        }
    }

    // Print augmented matrix at the start
    printf("Augmented matrix at start:\n");
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < 2 * n; j++) 
        {
            printf("(%lf, %lf) ", aug[i][j].real, aug[i][j].imag);
        }
        printf("\n");
    }

    for (int i = 0; i < n; i++) 
    {
        Complex pivot = aug[i][i];
        if (pivot.real == 0 && pivot.imag == 0) 
        {
            fprintf(stderr, "Matrix is singular and cannot be inverted.\n");
            for (int k = 0; k < n; k++) free(aug[k]);
            free(aug);
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < 2 * n; j++) 
        {
            aug[i][j].real /= pivot.real;
            aug[i][j].imag /= pivot.imag;
        }

        // Print matrix after pivot normalization
        printf("Augmented matrix after pivot normalization (row %d):\n", i);
        for (int x = 0; x < n; x++) 
        {
            for (int y = 0; y < 2 * n; y++) 
            {
                printf("(%lf, %lf) ", aug[x][y].real, aug[x][y].imag);
            }
            printf("\n");
        }

        for (int k = 0; k < n; k++) 
        {
            if (k != i) 
            {
                Complex factor = aug[k][i];
                for (int j = 0; j < 2 * n; j++) 
                {
                    aug[k][j].real -= factor.real * aug[i][j].real - factor.imag * aug[i][j].imag;
                    aug[k][j].imag -= factor.real * aug[i][j].imag + factor.imag * aug[i][j].real;
                }
            }
        }

        // Print matrix after eliminating column
        printf("Augmented matrix after eliminating column (row %d):\n", i);
        for (int x = 0; x < n; x++) 
        {
            for (int y = 0; y < 2 * n; y++) 
            {
                printf("(%lf, %lf) ", aug[x][y].real, aug[x][y].imag);
            }
            printf("\n");
        }
    }

    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < n; j++) 
        {
            out[i * n + j] = aug[i][j + n];
        }
    }

    for (int i = 0; i < n; i++) 
    {
        free(aug[i]);
    }
    free(aug);
}

void pseudo_inverse(Complex *A, Complex *pseudo_inv, int rows, int cols) 
{
    Complex AH[cols * rows];
    Complex AHA[cols * cols];
    Complex AHA_inv[cols * cols];

    conjugate_transpose(A, AH, rows, cols);
    print_matrix("A^H", AH, cols, rows);

    multiply_matrices(AH, A, AHA, cols, rows, cols);
    print_matrix("A^H * A", AHA, cols, cols);

    invert_matrix(AHA, AHA_inv, cols);
    print_matrix("(A^H * A)^-1", AHA_inv, cols, cols);

    multiply_matrices(AHA_inv, AH, pseudo_inv, cols, cols, rows);
}