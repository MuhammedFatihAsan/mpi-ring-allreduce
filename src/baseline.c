#include "utils.h"
#include "timing_utils.h"


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) printf("Usage: %s <number_of_elements>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    size_t count = atoi(argv[1]);

    float* send_buf = allocate_buffer(count);
    float* recv_buf = allocate_buffer(count);
    generate_dummy_data(send_buf, count);

    // barrier so all ranks start the timed region together (matches ring)
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = get_current_time();

    MPI_Reduce(send_buf, recv_buf, count, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Bcast(recv_buf, count, MPI_FLOAT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = get_current_time();
    double duration = calculate_elapsed_time(start_time, end_time);

    if (rank == 0) {
        printf("Baseline completed for %zu elements on %d ranks.\n", count, size);
        printf("Communication Time: %f seconds\n", duration);
        verify_results(recv_buf, count, (float)size);
    }

    free_buffer(send_buf);
    free_buffer(recv_buf);
    MPI_Finalize();

    return 0;
}
