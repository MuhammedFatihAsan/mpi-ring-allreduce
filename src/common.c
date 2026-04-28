#include "utils.h"
#include "timing_utils.h"


/* --- utils.h Fonksiyonları --- */

/**
 * Bellek tahsisi (malloc) işlemlerini standartlaştıran fonksiyon.
 * Hata durumunda programı güvenli bir şekilde sonlandırır.
 */
float* allocate_buffer(size_t size) {
    float* buffer = (float*)malloc(size * sizeof(float));
    if (!buffer) {
        fprintf(stderr, "Fatal Error: Memory allocation failed!\n");
        exit(1);
    }
    return buffer;
}

/**
 * Bellek boşaltma (free) işlemlerini gerçekleştiren fonksiyon.
 */
void free_buffer(float* data) {
    if (data != NULL) {
        free(data);
    }
}

/**
 * Her Rank için 1.0 değerlerinden oluşan test verisi üretir.
 */
void generate_dummy_data(float* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        data[i] = 1.0f;
    }
}

/**
 * İşlem sonuçlarını matematiksel olarak doğrular.
 * Toplam değerinin (float hassasiyetiyle) beklenen değere eşit olup olmadığına bakar.
 */
int verify_results(float* data, size_t size, float expected_value) {
    for (size_t i = 0; i < size; i++) {
        // fabs ile float karşılaştırma hatasını (epsilon) engelliyoruz
        if (fabs(data[i] - expected_value) > 1e-5) {
            printf("Verification: FAIL (Index %zu expected %f but got %f)\n", i, expected_value, data[i]);
            return 0; // Hata bulundu
        }
    }
    printf("Verification: PASS\n");
    return 1; // Başarılı
}

/* --- timing_utils.h Fonksiyonları --- */

/**
 * MPI_Wtime kullanarak o anki yüksek çözünürlüklü zamanı döndürür.
 */
double get_current_time() {
    return MPI_Wtime();
}

/**
 * İki zaman değeri arasındaki farkı (geçen süreyi) saniye cinsinden döndürür.
 */
double calculate_elapsed_time(double start, double end) {
    return end - start;
}