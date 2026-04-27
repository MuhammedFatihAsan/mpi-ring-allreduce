#include "utils.h"
#include "timing_utils.h"


int main(int argc, char** argv) {
    // 1. MPI Ortamını Başlat
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Argüman kontrolü: Veri boyutunu kullanıcıdan alıyoruz (Örn: 1024 eleman)
    if (argc < 2) {
        if (rank == 0) printf("Usage: %s <number_of_elements>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    size_t count = atoi(argv[1]);
    
    // 2. Bellek Ayırma ve Veri Hazırlama
    float* send_buf = allocate_buffer(count);
    float* recv_buf = allocate_buffer(count);
    
    // Herkes elindeki veriyi 1.0 ile doldurur
    generate_dummy_data(send_buf, count);

    // 3. Zaman Ölçümü Başlasın
    
    double start_time = get_current_time();

    // --- MERKEZİ TOPLAMA ALGORİTMASI ---
    
    // Step 1: Herkesteki veriyi Rank 0'da topla ve topla (SUM)
    MPI_Reduce(send_buf, recv_buf, count, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Step 2: Rank 0 sonucu herkese geri göndersin
    MPI_Bcast(recv_buf, count, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // ----------------------------------

    double end_time = get_current_time();
    double duration = calculate_elapsed_time(start_time, end_time);

    // 4. Doğrulama ve Sonuçları Yazdırma
    
    if (rank == 0) {
        printf("Baseline completed for %zu elements on %d ranks.\n", count, size);
        printf("Elapsed Time: %f seconds\n", duration);
        verify_results(recv_buf, count, (float)size);
    }

    // 5. Temizlik
    free_buffer(send_buf);
    free_buffer(recv_buf);
    MPI_Finalize();

    return 0;
}