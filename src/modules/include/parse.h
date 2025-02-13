/**
 * @file parse.h
 * @authors elmatree (https://edu.21-school.ru/profile/elmatree)
 * @authors karstarl (https://edu.21-school.ru/profile/karstarl)
 * @authors shericen (https://edu.21-school.ru/profile/shericen)
 * @brief Module header by model parsing
 * @version 1.0
 * @date 2024-12-19
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _PARSE_H_
#define _PARSE_H_

#include <ctype.h>

#include "./objects.h"

int parse_file(const char *filename, data_t *data);
data_t copy_data(data_t *object);
void remove_data(data_t *data);

#endif  // _PARSE_H_
