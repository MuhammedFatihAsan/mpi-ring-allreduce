#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

float* allocate_buffer(size_t size);
void free_buffer(float* data);

// fills with 1.0 so the expected sum after reduce is just the rank count
void generate_dummy_data(float* data, size_t size);

int verify_results(float* data, size_t size, float expected_value);

#endif
