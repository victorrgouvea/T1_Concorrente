#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h> 

#include "student.h"
#include "config.h"
#include "worker_gate.h"
#include "globals.h"
#include "table.h"
#include "queue.h"
#include "buffet.h"

void* student_run(void *arg)
{
    student_t *self = (student_t*) arg;
    table_t *tables  = globals_get_table();
    queue_t *queue = globals_get_queue();
    //buffet_t *b = globals_get_buffets();

    queue_insert(queue, self); // insere o estudante na fila

    /* Fica no loop até que seja retirado da fila */
    while (globals_get_id_estudante_entrada() != self->_id) {}
    printf("passou\n");
    worker_gate_insert_queue_buffet(self);
    // prints pra debug
    /*printf("%d\n", self->_id);
    printf("%d\n", self->left_or_right);
    printf("%d\n", self->_id_buffet);
    printf("%d\n", self->_buffet_position);
    printf("%d\n", b[self->_id_buffet].queue_left[0]);
    printf("%d\n", b[self->_id_buffet].queue_right[0]);*/
    student_serve(self);
    student_seat(self, tables);
    student_leave(self, tables);

    pthread_exit(NULL);
};

void student_seat(student_t *self, table_t *table)
{
    /*
    Percorro a lista de mesas até achar uma com um ligar vago
    Quando achar, decremento um da variável de lugares vagos e
    armazendo em qual mesa o aluno sentou
    */
    int num_mesas = globals_get_tables_number();
    
    int achou = 0;
    while (!achou) {
        for (int i = 0; i < num_mesas; i++) {
            pthread_mutex_lock(&(table[i].mutex_table));
            if (table[i]._empty_seats > 0) {
                table[i]._empty_seats--;
                pthread_mutex_unlock(&(table[i].mutex_table));
                // Armazeno o id da mesa que o estudante sentou
                // em id_buffet, ja que nao esta mais sendo usado
                self->_id_buffet = table[i]._id;
                achou = 1;
                break;
            }
        }
    }

}

void student_serve(student_t *self)
{
    /*
    INCOMPLETA
    Falta ver os mutex
    */
    buffet_t *buffet = globals_get_buffets();

    /* posicao = 5 indica que saiu do buffet */
    while (self->_buffet_position < 5) {
        
        /* Caso queria aquela comida, se serve dela */
        if (self->_wishes[self->_buffet_position] == 1) {
            // Espera a reposicao caso nao tenha comida
            while (!buffet[self->_id_buffet]._meal[self->_buffet_position]) {} // Espera a reposicao caso nao tenha comida
            pthread_mutex_lock(&(buffet[self->_id_buffet].mutex_meal[self->_buffet_position]));
            buffet[self->_id_buffet]._meal[self->_buffet_position] -= 1;  // Pega a comida
            pthread_mutex_unlock(&(buffet[self->_id_buffet].mutex_meal[self->_buffet_position]));
        }
        if (self->left_or_right == 'L' && self->_buffet_position < 4) {
            // Espera ate a proxima posicao da fila ser liberada
            while (buffet[self->_id_buffet].queue_left[(self->_buffet_position)+1]) {}
            // Talvez alguma solucao com semaforo aqui??
        } 
        else if (self->left_or_right == 'R' && self->_buffet_position < 4) {
            // Espera ate a proxima posicao da fila ser liberada
            while (buffet[self->_id_buffet].queue_right[(self->_buffet_position)+1]) {}
            // Talvez alguma solucao com semaforo aqui??
        }
        buffet_next_step(buffet, self);
    }
}
/* 
esta na fila?
verifica se quer a comida daquela posicao ou nao. se nao quiser, passa p/ a prox.
caso queira, verifica se tem. se tiver, pega. se nao tiver, espera
depois de pegar a comida, passa para a proxima posicao
quando passar da ultima posicao, vai sentar para comer
*/

void student_leave(student_t *self, table_t *table)
{
    /*
    Após o estudante terminar de comer, libera um assento da mesa em que ele estava
    */
    msleep(5000); // Tempo que o estudante esta comendo

    pthread_mutex_lock(&(table[self->_id_buffet].mutex_table));
    (table[self->_id_buffet]._empty_seats)++;
    pthread_mutex_unlock(&(table[self->_id_buffet].mutex_table));
}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

student_t *student_init()
{
    student_t *student = malloc(sizeof(student_t));
    student->_id = rand() % 1000;
    student->_buffet_position = -1;
    int none = TRUE;
    for (int j = 0; j <= 4; j++)
    {
        student->_wishes[j] = _student_choice();
        if(student->_wishes[j] == 1) none = FALSE;
    }

    if(none == FALSE){
        /* O estudante só deseja proteína */
        student->_wishes[3] = 1;
    }

    return student;
};

void student_finalize(student_t *self){
    free(self);
};


pthread_t students_come_to_lunch(int number_students)
{
    pthread_t lets_go;
    pthread_create(&lets_go, NULL, _all_they_come, &number_students);
    return lets_go;
}

/**
 * @brief Função (privada) que inicializa as threads dos alunos.
 * 
 * @param arg 
 * @return void* 
 */
void* _all_they_come(void *arg)
{
    int number_students = *((int *)arg);
    
    student_t *students[number_students];

    for (int i = 0; i < number_students; i++)
    {
        students[i] = student_init();                                               /* Estudante é iniciado, recebe um ID e escolhe o que vai comer*/
    }

    for (int i = 0; i < number_students; i++)
    {
        pthread_create(&students[i]->thread, NULL, student_run, students[i]);       /*  Cria as threads  */
    }

    for (int i = 0; i < number_students; i++)
    {
        pthread_join(students[i]->thread, NULL);                                    /*  Aguarda o término das threads   */
    }

    for (int i = 0; i < number_students; i++)
    {
        student_finalize(students[i]);                                              /*  Libera a memória de cada estudante  */
    }

    pthread_exit(NULL);
}

/**
 * @brief Função que retorna as escolhas dos alunos, aleatoriamente (50% para cada opção)
 *        retornando 1 (escolhido) 0 (não escolhido). É possível que um aluno não goste de nenhuma opção
 *         de comida. Nesse caso, considere que ele ainda passa pela fila, como todos aqueles que vão comer.
 * @return int 
 */
int _student_choice()
{
    float prob = (float)rand() / RAND_MAX;
    return prob > 0.51 ? 1 : 0;
}