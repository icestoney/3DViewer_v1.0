/**
 * @file objects.h
 * @authors elmatree (https://edu.21-school.ru/profile/elmatree)
 * @authors karstarl (https://edu.21-school.ru/profile/karstarl)
 * @authors shericen (https://edu.21-school.ru/profile/shericen)
 * @brief Header containing model data objects
 * @version 1.0
 * @date 2024-12-19
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <stdbool.h>

#include "./matrix.h"

/// @brief Contains all 3D model data
typedef struct {
  int vertex_count;    ///< Number of model vertices
  matrix_t vertexes;   ///< Model vertex array
  int facet_count;     ///< Number of model facets
  int *facets;         ///< Model facets array
  int full_cnt;        ///< Facets arrays size
  float max_position;  ///< Largest modulo coordinate
} data_t;

/// @brief Contains 3D model transformation matrices
typedef struct {
  float *current;   ///< Effective transformation matrix
  float *identity;  ///< Identity matrix
  float *move;      ///< Translate matrix
  float *rotate_x;  ///< X-axis rotation matrix
  float *rotate_y;  ///< Y-axis rotation matrix
  float *rotate_z;  ///< Z-axis rotation matrix
  float *scale;     ///< Scale matrix
} afinne_t;

#endif  // _OBJECTS_H_