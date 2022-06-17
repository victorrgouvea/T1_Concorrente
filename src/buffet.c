#include <stdlib.h>
#include <pthread.h>
#include "buffet.h"
#include "config.h"
#include "globals.h"


void *buffet_run(void *arg)
{   
    buffet_t *self = (buffet_t*) arg;
    int num_students = globals_get_students();
    
    /* 
    O buffet funciona enquanto o numero de estudantes que passaram
    pelo buffet for diferente do numero total de estudantes
    Quando esses numeros forem iguais, significa que todos os 
    estudantes ja se serviram e sairam do buffet, e ele pode ser finalizado
    */
    while (num_students != globals_get_passaram_pelo_buffet())
    {
        /* Cada buffet possui: Arroz, Feijão, Acompanhamento, Proteína e Salada */
        /* Máximo de porções por bacia (40 unidades). */
        _log_buffet(self);

        msleep(5000); /* Pode retirar este sleep quando implementar a solução! */
    }

    // Apos terminar, cada buffet faz o destroy dos seus mutex de cada bacia de comida
    for (int j = 0; j < 5; j++) {
        pthread_mutex_destroy(&(self->mutex_meal[j]));
    }
    pthread_exit(NULL);
}

void buffet_init(buffet_t *self, int number_of_buffets)
{
    int i = 0, j = 0;
    globals_set_num_buffets(number_of_buffets);
    for (i = 0; i < number_of_buffets; i++)
    {
        /*A fila possui um ID*/
        self[i]._id = i;

        /* Inicia com 40 unidades de comida em cada bacia */
        for(j = 0; j < 5; j++) {
            self[i]._meal[j] = 40;
            // Aproveito esse for para iniciar os mutex de cada bacia de comida
            pthread_mutex_init(&(self[i].mutex_meal[j]), NULL);
        }
        
        for(j= 0; j< 5; j++){
             /* A fila esquerda do buffet possui cinco posições. */
            self[i].queue_left[j] = 0;
            /* A fila esquerda do buffet possui cinco posições. */
            self[i].queue_right[j] = 0;
        }

        pthread_create(&self[i].thread, NULL, buffet_run, &self[i]);
    }
}


int buffet_queue_insert(buffet_t *self, student_t *student)
{
    /* Se o estudante vai para a fila esquerda */
    if (student->left_or_right == 'L') 
    {
        /* Verifica se a primeira posição está vaga */
        if (!self[student->_id_buffet].queue_left[0])
        {
            self[student->_id_buffet].queue_left[0] = student->_id;
            student->_buffet_position = 0;
            return TRUE;
        }
        return FALSE;
    }
    else
    {   /* Se o estudante vai para a fila direita */
        if (!self[student->_id_buffet].queue_right[0])
        {
            /* Verifica se a primeira posição está vaga */
            self[student->_id_buffet].queue_right[0] = student->_id;
            student->_buffet_position = 0;
            return TRUE;
        }
        return FALSE;
    }
}


void buffet_next_step(buffet_t *self, student_t *student)
{
    /* Se estudante ainda precisa se servir de mais alguma coisa... */
    if (student->_buffet_position + 1 < 5)
    {    /* Está na fila esquerda? */
        if (student->left_or_right == 'L')
        {   /* Caminha para a posição seguinte da fila do buffet.*/
            int position = student->_buffet_position;
            self[student->_id_buffet].queue_left[position] = 0;
            self[student->_id_buffet].queue_left[position + 1] = student->_id;
            student->_buffet_position = student->_buffet_position + 1;
        }else /* Está na fila direita? */
        {   /* Caminha para a posição seguinte da fila do buffet.*/
            int position = student->_buffet_position;
            self[student->_id_buffet].queue_right[position] = 0;
            self[student->_id_buffet].queue_right[position + 1] = student->_id;
            student->_buffet_position = student->_buffet_position + 1;
        }
    }
    /*
    Caso esteja na ultima posicao do buffet, não devemos definir
    a proxima posicao do estudante, ja que ela não existe, logo 
    temos as mesma operações do if acima, tirando essa definição da
    proxima posicao
    Nesse caso, tambem temos que modificar a variável global de quantos
    estudantes passaram pelo buffet, pois ele esta na ultima posicao e 
    pronto para sair
    */
    else 
    {
        if (student->left_or_right == 'L')
        {   /* Zera a ultima posicao e sai do buffet*/
            int position = student->_buffet_position;
            self[student->_id_buffet].queue_left[position] = 0;
            student->_buffet_position = student->_buffet_position + 1;
            // Atualiza a variável de quantos sairam do buffet
            // Utlizo um mutex, pois varios estudantes podem
            // querer atualizar a variável ao mesmo tempo
            pthread_mutex_lock(globals_get_mutex_passaram());
            int x = globals_get_passaram_pelo_buffet();
            x += 1;
            globals_set_passaram_pelo_buffet(x);
            pthread_mutex_unlock(globals_get_mutex_passaram());
        }else /* Está na fila direita? */
        {   /* Zera a ultima posicao e sai do buffet*/
            int position = student->_buffet_position;
            self[student->_id_buffet].queue_right[position] = 0;
            student->_buffet_position = student->_buffet_position + 1;
            // Atualiza a variável de quantos sairam do buffet
            // Utlizo um mutex, pois varios estudantes podem
            // querer atualizar a variável ao mesmo tempo
            pthread_mutex_lock(globals_get_mutex_passaram());
            int x = globals_get_passaram_pelo_buffet();
            x += 1;
            globals_set_passaram_pelo_buffet(x);
            pthread_mutex_unlock((globals_get_mutex_passaram()));
        }
    }
}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

void buffet_finalize(buffet_t *self, int number_of_buffets)
{
    /* Espera as threads se encerrarem...*/
    for (int i = 0; i < number_of_buffets; i++)
    {
        pthread_join(self[i].thread, NULL);
    }
    
    /*Libera a memória.*/
    free(self);
}


void _log_buffet(buffet_t *self)
{
    /* Prints do buffet */
    int *ids_left = self->queue_left; 
    int *ids_right = self->queue_right; 

    printf("\n\n\u250F\u2501 Queue left: [ %d %d %d %d %d ]\n", ids_left[0],ids_left[1],ids_left[2],ids_left[3],ids_left[4]);
    fflush(stdout);
    printf("\u2523\u2501 BUFFET %d = [RICE: %d/40 BEANS:%d/40 PLUS:%d/40 PROTEIN:%d/40 SALAD:%d/40]\n",
           self->_id, self->_meal[0], self->_meal[1], self->_meal[2], self->_meal[3], self->_meal[4]);
    fflush(stdout);
    printf("\u2517\u2501 Queue right: [ %d %d %d %d %d ]\n", ids_right[0],ids_right[1],ids_right[2],ids_right[3],ids_right[4]);
    fflush(stdout);
}