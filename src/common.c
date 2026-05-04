#include "utils.h"
#include "timing_utils.h"


float* allocate_buffer(size_t size) {
    float* buffer = (float*)malloc(size * sizeof(float));
    if (!buffer) {
        fprintf(stderr, "Fatal Error: Memory allocation failed!\n");
        exit(1);
    }
    return buffer;
}

void free_buffer(float* data) {
    if (data != NULL) {
        free(data);
    }
}

void generate_dummy_data(float* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        data[i] = 1.0f;
    }
}

int verify_results(float* data, size_t size, float expected_value) {
    for (size_t i = 0; i < size; i++) {
        // float compare with epsilon, otherwise large sums fail
        if (fabs(data[i] - expected_value) > 1e-5) {
            printf("Verification: FAIL (Index %zu expected %f but got %f)\n", i, expected_value, data[i]);
            return 0;
        }
    }
    printf("Verification: PASS\n");
    return 1;
}

double get_current_time() {
    return MPI_Wtime();
}

double calculate_elapsed_time(double start, double end) {
    return end - start;
}
