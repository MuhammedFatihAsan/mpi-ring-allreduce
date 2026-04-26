#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/**
 * Bellek tahsisi (malloc) işlemlerini standartlaştıran fonksiyon. 
 * Verilen boyutta bir float dizisi oluşturur.
 */
float* allocate_buffer(size_t size);

/**
 * Bellek boşaltma (free) işlemlerini standartlaştıran fonksiyon. 
 */
void free_buffer(float* data);

/**
 * Her Rank için tahmin edilebilir veriler (örneğin 1.0) üretir.
 * Bu sayede toplama işleminin doğruluğu kolayca test edilebilir.
 */
void generate_dummy_data(float* data, size_t size);

/**
 * İşlemlerin doğruluğunu kontrol eden mekanizma.
 * Örneğin 4 Node'lu sistemde sonucun 4.0 olup olmadığına bakar.
 */
int verify_results(float* data, size_t size, float expected_value);

#endif