#include <stdlib.h>

#include "chef.h"
#include "config.h"
#include "buffet.h"
#include "worker_gate.h"
#include "globals.h"

// Chef ações descritas:
//
// 1 - Verifica se as bacias de comidas estão vazias
//
// Loop passando pelos buffets e depois um loop iterno para verificar
// cada bacia. Cada verificação da bacia, deve travar o consumo do aluno
// após verificado, a bacia deve ser liberada para o consumo
// 
// Condicionais:
//
// Bacia está vazia:
// O controle da bacia continua segurando o consumo do aluno
// Chef chama a função por comida
// Depois da comida preenchida ele libera o consumo do aluno
// 
// Bacia está cheia:
// Libera o consumo do aluno
//
// 2 - Coloca comida
//
// Reatribui o valor 40 para o contador de cada bacia
//
// 3 -Depois de todos os alunos se servirem, ele vai embora
// 
// Mecanismo de verificação se tem algum aluno na fila
// Destruição dos controles envolvidos nas sua ações
// Pelo fato de não ser indicado mexer no finalize
// a condicional de finalização ou não, deve ficar no run
//
// (Dúvida: só coloca quando ficam vazias?)
// (Dúvida: somente o chef que retorna a verificação ou aluno também
// tem essa capacidade?)


void *chef_run()
{
    while (globals_get_buffets() == NULL);
    // verifica se ainda tem alunos na fila, para assim
    // saber se deve trabalhar ou não.
    int num_students = globals_get_students();
    while (num_students != globals_get_passaram_pelo_buffet())
    {   
        // checa a comida e a partir disso faz as ações
        chef_check_food();
        msleep(5000); /* Pode retirar este sleep quando implementar a solução! */
    }
    
    pthread_exit(NULL);
}


void chef_put_food(int buffet_num, int comida_num)
{
    // Pegar a lista dos buffets
    buffet_t *buffets = globals_get_buffets();
    // Atribui o valor de 40 para bacia que estava vazia
    buffets[buffet_num]._meal[comida_num] = 40; 
}
void chef_check_food()
{  
    // Pegar a lista dos buffets
    buffet_t *buffets = globals_get_buffets();
    // Pegar a quantidade de buffets
    int num_buffets = globals_get_num_buffets();
    
    int i, j;
    // Este primeiro for varre a lista de buffets
    for (i = 0; i < num_buffets; i++) 
    {   
        // O segundo for varre a lista de comidas 
        // em cada buffet.
        for (j = 0; j < 5; j++) 
        {   
            // O mecanismo utilizado abaixo serve
            // para que não haja alteração no valor
            // da quantidade de comida, enquanto a verificação
            // e edição seja feita pelo chef.
            // Esse mutex também está presente, no local
            // onde o estudante se serve de comida.

            pthread_mutex_lock(&(buffets[i].mutex_meal[j]));
            if(buffets[i]._meal[j] == 0) 
            {
                chef_put_food(i, j);
            }
            pthread_mutex_unlock(&(buffets[i].mutex_meal[j]));

        }

    }
}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

void chef_init(chef_t *self)
{
    pthread_create(&self->thread, NULL, chef_run, NULL);
}

void chef_finalize(chef_t *self)
{
    pthread_join(self->thread, NULL);
    free(self);
}