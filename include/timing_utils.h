#ifndef TIMING_UTILS_H
#define TIMING_UTILS_H

#include <mpi.h>

/**
 * İşlemin başladığı anı kaydeder.
 * MPI_Wtime() fonksiyonunu kullanarak o anki saniyeyi döndürür. 
 */
double get_current_time();

/**
 * Başlangıç ve bitiş zamanı arasındaki farkı hesaplar.
 */
double calculate_elapsed_time(double start, double end);

#endif