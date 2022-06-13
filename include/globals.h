#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "queue.h"
#include "table.h"
#include "buffet.h"

/**
 * @brief Inicia uma fila (de modo global)
 * 
 * @param queue 
 */
extern void globals_set_queue(queue_t *queue);

/**
 * @brief Retorna uma fila (de modo global)
 * 
 * @return queue_t* 
 */
extern queue_t *globals_get_queue();

/**
 * @brief Insere o número de alunos (de modo global)
 * 
 */
extern void globals_set_students(int number);

/**
 * @brief Retorna o número de alunos (de modo global)
 * 
 * @return int 
 */

extern int globals_get_students();

/**
 * @brief Inicia um array de mesas (de modo global).
 * 
 * @param t 
 */
extern void globals_set_table(table_t *t);

/**
 * @brief Retorna um array de mesas (de modo global)
 * 
 * @return table_t* 
 */
extern table_t *globals_get_table();


/**
 * @brief Finaliza todas as variáveis globais.
 * 
 */
extern void globals_finalize();

/**
 * @brief Inicia um array de buffets (de modo global)
 * 
 */
extern void globals_set_buffets(buffet_t *buffets_ref);

/**
 * @brief Retorna um array de buffets (de modo global)
 * 
 * @return buffet_t* 
 */
extern buffet_t *globals_get_buffets();

// Inicia o numero de alunos
extern void globals_set_num_buffets(int number);

// Retorna o numero de alunos
extern int globals_get_num_buffets();

// Armazena o id do estudante que passa pela catraca
extern void globals_set_id_estudante_entrada(int number);

// Retorna o id do estudante que passa pela catraca
extern int globals_get_id_estudante_entrada();

// Armazena o número total de mesas
extern void globals_set_tables_number(int number);

// Retorna o número total de mesas
extern int globals_get_tables_number();

// Armazena o numero de assentos por mesa
extern void globals_set_seats_per_table(int number);

// Retorna o numero de assentos por mesa
extern int globals_get_seats_per_table();

// Init dos mutex das mesas
extern void tables_mutex_init(table_t *t);

// Destroy dos mutex das mesas
extern void tables_mutex_destroy(table_t *t);

// Destroy dos mutex do buffet
//extern void buffet_mutex_destroy(buffet_t *b);

#endif