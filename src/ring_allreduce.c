#include "utils.h"
#include "timing_utils.h"

size_t get_chunk_size(size_t count, int size, int chunk_id);

size_t get_chunk_start(size_t count, int size, int chunk_id);

int main(int argc, char** argv) {
    
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // The program needs the array size as an argument.
    if (argc < 2) {
        if (rank == 0) {
            printf("Usage: %s <number_of_elements>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    size_t count = atoi(argv[1]);

    // Each rank creates its own local data.
    float* data = allocate_buffer(count);
    generate_dummy_data(data, count);

    double total_start_time = get_current_time();

    // Find left and right neighbors in the ring.
    int left = (rank - 1 + size) % size;
    int right = (rank + 1) % size;

    // The biggest chunk is needed for safe receiving.
    size_t max_chunk_size = get_chunk_size(count, size, 0);
    float* temp_buffer = allocate_buffer(max_chunk_size);

    MPI_Barrier(MPI_COMM_WORLD);
    double communication_start_time = get_current_time();

    // Reduce-scatter phase.
    for (int step = 0; step < size - 1; step++) {
        
        int send_chunk = (rank - step + size) % size;
        int recv_chunk = (rank - step - 1 + size) % size;

        size_t send_start = get_chunk_start(count, size, send_chunk);
        size_t send_count = get_chunk_size(count, size, send_chunk);

        size_t recv_start = get_chunk_start(count, size, recv_chunk);
        size_t recv_count = get_chunk_size(count, size, recv_chunk);

        MPI_Sendrecv(
            data + send_start,     // data to send
            send_count,            // number of floats to send
            MPI_FLOAT,             // data type
            right,                 // send to right neighbor
            0,                     // message tag
            temp_buffer,           // received data is stored here
            recv_count,            // number of floats to receive
            MPI_FLOAT,             // data type
            left,                  // receive from left neighbor
            0,                     // message tag
            MPI_COMM_WORLD,        // MPI communicator
            MPI_STATUS_IGNORE      // ignore message status
        );

        // Add received values to the correct chunk.
        for (size_t i = 0; i < recv_count; i++) {
            data[recv_start + i] += temp_buffer[i];
        }
    }

    // Allgather phase.
    for (int step = 0; step < size - 1; step++) {
    
        int send_chunk = (rank + 1 - step + size) % size;
        int recv_chunk = (rank - step + size) % size;

        size_t send_start = get_chunk_start(count, size, send_chunk);
        size_t send_count = get_chunk_size(count, size, send_chunk);

        size_t recv_start = get_chunk_start(count, size, recv_chunk);
        size_t recv_count = get_chunk_size(count, size, recv_chunk);

        MPI_Sendrecv(
            data + send_start,     // data to send
            send_count,            // number of floats to send
            MPI_FLOAT,             // data type
            right,                 // send to right neighbor
            1,                     // message tag
            temp_buffer,           // received data is stored here
            recv_count,            // number of floats to receive
            MPI_FLOAT,             // data type
            left,                  // receive from left neighbor
            1,                     // message tag
            MPI_COMM_WORLD,        // MPI communicator
            MPI_STATUS_IGNORE      // ignore message status
        );

        // Copy the received completed chunk.
        for (size_t i = 0; i < recv_count; i++) {
            data[recv_start + i] = temp_buffer[i];
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double communication_end_time = get_current_time();
    double total_end_time = get_current_time();

    double communication_duration = calculate_elapsed_time(communication_start_time, communication_end_time);
    double total_duration = calculate_elapsed_time(total_start_time, total_end_time);

    if (rank == 0) {
        printf("Ring-Allreduce completed for %zu elements on %d ranks.\n", count, size);
        printf("Communication Time: %f seconds\n", communication_duration);
        printf("Setup + Communication Time: %f seconds\n", total_duration);
        verify_results(data, count, (float)size);
    }

    free_buffer(temp_buffer);
    free_buffer(data);

    MPI_Finalize();
    return 0;
}

size_t get_chunk_size(size_t count, int size, int chunk_id) {
    
    size_t base_chunk_size = count / size;
    size_t remainder = count % size;

    size_t chunk_size = base_chunk_size;
    if ((size_t)chunk_id < remainder) {
        chunk_size++;
    }

    return chunk_size;
}

size_t get_chunk_start(size_t count, int size, int chunk_id) {
    
    size_t base_chunk_size = count / size;
    size_t remainder = count % size;

    size_t chunk_start = chunk_id * base_chunk_size;
    if ((size_t)chunk_id < remainder) {
        chunk_start += chunk_id;
    } else {
        chunk_start += remainder;
    }

    return chunk_start;
}
