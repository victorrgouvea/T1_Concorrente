#include <stdlib.h>

#include "chef.h"
#include "config.h"

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
    /* Insira sua lógica aqui */
    while (TRUE)
    {
        msleep(5000); /* Pode retirar este sleep quando implementar a solução! */
    }
    
    pthread_exit(NULL);
}


void chef_put_food()
{
    /* Insira sua lógica aqui */
}
void chef_check_food()
{
    /* Insira sua lógica aqui */
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