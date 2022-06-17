#include <stdlib.h>
#include <semaphore.h>

#include "worker_gate.h"
#include "globals.h"
#include "config.h"
#include "queue.h"
#include "student.h"
#include "table.h"

/*
Semaforo para garantir que só um estudante por vez 
está passando pela catraca
*/
sem_t catraca;

int worker_gate_look_queue()
{
    /* 
    Verifica se ainda tem alguem na fila
    Caso tenha retorna 1, se não, 0
    */
    queue_t *fila = globals_get_queue();
    if (fila->_length > 0) {
        return 1;
    }
    return 0;
}

int worker_gate_remove_student()
{
    /*
    Retira o proximo estudante da fila
    e retorna o seu id
    */
    queue_t *fila = globals_get_queue();
    student_t *estudante = queue_remove(fila);
    return estudante->_id;
}

int worker_gate_look_buffet()
{
    /* 
    Verifica se a primeira posicao de alguma fila de um dos buffets está livre
    Caso esteja, retorna o id do buffet
    Se não encontrar nenhum livre, retorna -1
    */
    buffet_t *lista_buffet = globals_get_buffets();
    int num_buffets = globals_get_num_buffets();
    int buffet_livre = -1;
        for (int i = 0; i < num_buffets; i++) {
            // Se a primeira posicao de uma das filas do buffet for 0
            // aquele buffet está livre
            if (!lista_buffet[i].queue_left[0] || !lista_buffet[i].queue_right[0]) {
                // Defino o id do buffet livre
                buffet_livre = lista_buffet[i]._id;
                break;
            }
        }
    return buffet_livre;
}

void *worker_gate_run(void *arg)
{
    int number_students;
    int buffet_livre;
    int id_estudante;

    number_students = *((int *)arg);

    /*
    Enquanto o numero de estudantes for maior de 0, ainda
    temos estudantes na fila
    Apos retirarmos o ultimo estudante da fila, esse numero
    sera 0 e o worker gate pode encerrar
    */
    while (number_students > 0)
    {
        /* Testa se tem alguem na fila e se tem algum buffet livre */
        buffet_livre = worker_gate_look_buffet();
        if (worker_gate_look_queue() && buffet_livre > -1) {
            /*
            Deixa um estudante passar pela catraca, retira ela da fila
            e salva o seu id em uma variável global, que vai indicar 
            qual estudante está entrando no RU
            */
            sem_wait(&catraca);
            id_estudante = worker_gate_remove_student();
            globals_set_id_estudante_entrada(id_estudante);
            /* Após retirar um estudante da fila, subtrai 1 do numero de estudantes */
            number_students -= 1;
        }
        msleep(5000); /* Pode retirar este sleep quando implementar a solução! */
    }
    pthread_exit(NULL);
}

void worker_gate_init(worker_gate_t *self)
{
    int number_students = globals_get_students();
    table_t *mesas = globals_get_table();
    // Aproveito o init do worker gate para iniciar alguns mutex
    // usados em outras partes do código e também o semaforo da catraca
    tables_mutex_init(mesas);
    pthread_mutex_init(globals_get_mutex_passaram(), NULL);
    sem_init(&catraca, 0, 1);
    pthread_create(&self->thread, NULL, worker_gate_run, &number_students);
}

void worker_gate_finalize(worker_gate_t *self)
{
    pthread_join(self->thread, NULL);
    // Aproveito o finalize para finalizar o semaforo da catraca,
    // alguns mutex e as globais
    sem_destroy(&catraca);
    pthread_mutex_destroy(globals_get_mutex_passaram());
    globals_finalize();
    free(self);
}

void worker_gate_insert_queue_buffet(student_t *student)
{
    /*
    Procura por uma fila disponivel em algum buffet
    Caso encontre, define o id do buffet e qual fila ele vai entrar
    Apos isso, o estudante é inserido no buffet determinado
    */
    buffet_t *lista_buffet = globals_get_buffets();
    int num_buffets = globals_get_num_buffets();
    for (int i = 0; i < num_buffets; i++) {
        if (!lista_buffet[i].queue_left[0]) {
            student->left_or_right = 'L';
            student->_id_buffet = lista_buffet[i]._id;
            break;
        } else if (!lista_buffet[i].queue_right[0]) {
            student->left_or_right = 'R';
            student->_id_buffet = lista_buffet[i]._id;
            break;
        }
    }
    // insere o estudante na fila do buffet determinado
    buffet_queue_insert(lista_buffet, student);
    sem_post(&catraca); // post no final para liberar para o próxima passar pela catraca
}
