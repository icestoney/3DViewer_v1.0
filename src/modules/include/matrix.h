/**
 * @file matrix.h
 * @authors elmatree (https://edu.21-school.ru/profile/elmatree)
 * @authors karstarl (https://edu.21-school.ru/profile/karstarl)
 * @authors shericen (https://edu.21-school.ru/profile/shericen)
 * @brief Header of the module for working with matrices
 * @version 1.0
 * @date 2024-12-19
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief Number of vertex coordinates
#define V_CNT 3

/// @brief Transformation matrix dimension
#define TR_MX_SIZE 4

/// @brief Matrix struct
typedef struct {
  float *matrix;  ///< Matrix array
  int rows;       ///< Number of rows
  int cols;       ///< Number of columns
} matrix_t;

matrix_t mx_create(int rows, int cols);
void mx_remove(matrix_t *mx);
void mx_copy(float *copy, float *data);
void mx_mult_vector(float *data, float *vertexes, float *vector, int rows);
void mx_mult_4x4(float *current, const float *mul);

#endif  //_MATRIX_H_