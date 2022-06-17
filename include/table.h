#ifndef __TABLE_H__
#define __TABLE_H__

#include <pthread.h>

typedef struct table_t
{
        int _id;                         /* ID da mesa */
        int _empty_seats;               /* Quantidade de lugares vazios */
        int _max_seats;                 /* Capacidade máxima de cada mesa*/
        pthread_mutex_t mutex_table;    // Mutex de cada mesa, para evitar que
                                        // várias threads de estudantes tentem acessar
                                        // os valores de uma mesa ao mesmo tempo
} table_t; 

/**
* @brief Inicia uma mesa.
* 
* @param self 
*/

extern table_t *table_init(int number_of_tables, int seats_per_table);

#endif