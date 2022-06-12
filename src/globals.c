#include <stdlib.h>
#include "globals.h"

queue_t *students_queue = NULL;
table_t *table = NULL;
buffet_t *buffets_ref = NULL;

int students_number = 0;
int buffets_number = 0;
int id_estudante_entrada = 0;
int tables_number = 0;
int seats_per_table = 0;

void globals_set_queue(queue_t *queue)
{
    students_queue = queue;
}

queue_t *globals_get_queue()
{
    return students_queue;
}

void globals_set_table(table_t *t)
{
    table = t;
}

table_t *globals_get_table()
{
    return table;
}


void globals_set_students(int number)
{
    students_number = number;
}

int globals_get_students()
{
    return students_number;
}

void globals_set_buffets(buffet_t *buffets)
{
    buffets_ref = buffets;
}

buffet_t *globals_get_buffets()
{
    return buffets_ref;
}

void globals_set_num_buffets(int number)
{
    buffets_number = number;
}

int globals_get_num_buffets()
{
    return buffets_number;
}

void globals_set_id_estudante_entrada(int number)
{
    id_estudante_entrada = number;
}

int globals_get_id_estudante_entrada()
{
    return id_estudante_entrada;
}

void globals_set_tables_number(int number)
{
    tables_number = number;
}

int globals_get_tables_number()
{
    return tables_number;
}

void globals_set_seats_per_table(int number)
{
    seats_per_table = number;
}

int globals_get_seats_per_table()
{
    return seats_per_table;
}

/**
 * @brief Finaliza todas as variáveis globais que ainda não foram liberadas.
 *  Se criar alguma variável global que faça uso de mallocs, lembre-se sempre de usar o free dentro
 * dessa função.
 */
void globals_finalize()
{
    free(table);
}