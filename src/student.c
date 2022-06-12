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

    queue_insert(queue, self); // insere o estudante na fila
    
    /* Fica no loop até que seja retirado da fila */
    while (globals_get_id_estudante_entrada() != self->_id) {}
    
    worker_gate_insert_queue_buffet(self);
    student_serve(self);
    student_seat(self, tables);
    student_leave(self, tables);

    pthread_exit(NULL);
};

void student_seat(student_t *self, table_t *table)
{
    /* Insira sua lógica aqui */
    table_t *mesas = globals_get_table();

}

void student_serve(student_t *self)
{
    /*
    INCOMPLETA
    Falta ver os mutex
    */
    buffet_t *lista_buffet = globals_get_buffets();
    buffet_t buffet = lista_buffet[self->_id_buffet];

    /* posicao = 5 indica que saiu do buffet */
    while (self->_buffet_position < 5) {
        
        /* Caso queria aquela comida, se serve dela */
        if (self->_wishes[self->_buffet_position] == 1) {
            // MUTEX ??
            while (!buffet._meal[self->_buffet_position]) {} // Espera a reposicao caso nao tenha comida
            --buffet._meal[self->_buffet_position];  // Pega a comida
            // MUTEX ??
        }
        if (self->left_or_right == "L") {
            // Espera ate a proxima posicao da fila ser liberada
            while (buffet.queue_left[(self->_buffet_position)+1]) {}
            // Talvez alguma solucao com semaforo aqui??
        } 
        else if (self->left_or_right == "R") {
            // Espera ate a proxima posicao da fila ser liberada
            while (buffet.queue_right[(self->_buffet_position)+1]) {}
            // Talvez alguma solucao com semaforo aqui??
        }
        buffet_next_step(lista_buffet, self);
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
    /* Insira sua lógica aqui */
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