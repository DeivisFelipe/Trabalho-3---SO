#include "processos.h"
#include "cpu_estado.h"
#include "mem.h"
#include "es.h"
#include "tela.h"
#include <stdlib.h>
#include <stdbool.h>

// Estrutura base de um processo
struct processo_t {
    int id; // ID 0 é para o processo principal do sistema
    estado_t estado;
    acesso_t tipo_bloqueio;
    int terminal_bloqueio;
    cpu_estado_t *cpue;
    processo_t *proximo;
    // Memoria
    int inicio_memoria;
    int fim_memoria;
    tab_pag_t *tab_pag;
    // Estatisticas
    int tempo_em_execucao;
    int tempo_em_bloqueio;
    int tempo_em_pronto;
    int numero_execucoes;
    int numero_bloqueios;
    int numero_desbloqueios;
    int tempo_inicio;
    int numero_preempcoes;
    // Quantum
    int quantum;
};

// Função que cria o primeiro processo da lista de processos
processo_t* processos_cria(int id, estado_t estado , mem_t *mem, int inicio_memoria, int fim_memoria, cpu_estado_t *cpu, int tempo_inicio, int tamanho_pagina, int numero_de_paginas){
  processo_t *self = malloc(sizeof(*self));
  self->id = id;
  self->estado = estado;
  self->cpue = cpue_cria();
  self->proximo = NULL;
  self->inicio_memoria = inicio_memoria;
  self->fim_memoria = fim_memoria;
  self->terminal_bloqueio = -1;
  self->tipo_bloqueio = leitura;
  self->tempo_em_execucao = 0;
  self->tempo_em_bloqueio = 0;
  self->tempo_em_pronto = 0;
  self->numero_bloqueios = 0;
  self->numero_desbloqueios = 0;
  self->numero_execucoes = 1;
  self->tempo_inicio = tempo_inicio;
  self->numero_preempcoes = 0;
  self->quantum = 0;
  self->tab_pag = tab_pag_cria(numero_de_paginas, tamanho_pagina);


  cpue_copia(cpu, self->cpue);
  //mem_printa(mem, inicio_memoria, fim_memoria);
  return self;
}

// Função que insere um novo processo a lista de processos
processo_t *processos_insere(processo_t *lista, int id, estado_t estado, int inicio_memoria, int fim_memoria, cpu_estado_t *cpu, int tempo_inicio, int quantum, int tamanho_pagina, int numero_de_paginas){
  processo_t *novo = malloc(sizeof(*novo));
  novo->id = id;
  novo->estado = estado;
  novo->cpue = cpue_cria();
  novo->proximo = NULL;
  novo->inicio_memoria = inicio_memoria;
  novo->fim_memoria = fim_memoria;
  novo->terminal_bloqueio = -1;
  novo->tipo_bloqueio = leitura;
  novo->tempo_em_execucao = 0;
  novo->tempo_em_bloqueio = 0;
  novo->tempo_em_pronto = 0;
  novo->numero_bloqueios = 0;
  novo->numero_desbloqueios = 0;
  novo->numero_execucoes = 1;
  novo->tempo_inicio = tempo_inicio;
  novo->numero_preempcoes = 0;
  novo->quantum = quantum;
  novo->tab_pag = tab_pag_cria(numero_de_paginas, tamanho_pagina);


  cpue_copia(cpu, novo->cpue);
  if(lista->proximo == NULL){
    lista->proximo = novo;
  }else{
    processo_t *temp = lista->proximo;
    while(temp->proximo != NULL){
      temp = temp->proximo;
    }
    temp->proximo = novo;
  }
  return lista;
}

// Função que imprime a lista de processos
void processos_printa(processo_t *lista){
  processo_t *temp = lista;
  while(temp != NULL){
    t_printf("ID: %d, Estado: %d, Inicio: %d, Fim: %d, Bloqueio: %d, Tipo: %d", temp->id, temp->estado, temp->inicio_memoria, temp->fim_memoria, temp->terminal_bloqueio, temp->tipo_bloqueio);
    temp = temp->proximo;
  }
}

// Retorna a tabela de paginação do processo atual
tab_pag_t* processos_tabela_de_pag(processo_t *self){
  return self->tab_pag;
}

// Remove um processo
void processos_remove(processo_t *lista, processo_t *atual){
  processo_t *temp = lista;
  if(temp->proximo == NULL){
    free(temp->cpue);
    tab_pag_destroi(temp->tab_pag);
    free(temp);
    return;
  }
  while (temp->proximo != NULL && temp->proximo != atual) {
    temp = temp->proximo;
  }
  temp->proximo = atual->proximo;
  free(atual->cpue);
  tab_pag_destroi(temp->tab_pag);
  t_printf("estou aqui");
  free(atual);
}

// Atualiza os dados de um processo que já existe dentro da lista de processos pelo id
void processos_atualiza_dados(processo_t *lista, int id, estado_t estado, cpu_estado_t *cpu){
  processo_t *temp = lista;
  while (temp != NULL && temp->id != id)
  {
    temp = temp->proximo;
  }
  if(temp != NULL){
    // Atualiza o numero de bloqueios
    if(estado == BLOQUEADO && (temp->estado == PRONTO || temp->estado == EXECUCAO)){
      temp->numero_bloqueios++;
    }else if(estado == PRONTO && temp->estado == EXECUCAO){
      temp->numero_preempcoes++;
    }else if(estado == EXECUCAO && temp->estado == PRONTO){
      temp->numero_execucoes++;
    }

    temp->estado = estado;
    cpue_copia(cpu, temp->cpue);
  }  
}

// Verifica desbloqueio de processo
void processos_desbloqueia(processo_t *lista, es_t *estrada_saida){
  processo_t *temp = lista;
  while(temp != NULL){
    if(temp->estado == BLOQUEADO){
      if(es_pronto(estrada_saida, temp->terminal_bloqueio, temp->tipo_bloqueio)){
        temp->numero_desbloqueios++;
        temp->estado = PRONTO;
      }
    }
    temp = temp->proximo;
  }
}

// Bota o processo no fim da fila
void processos_bota_fim(processo_t *lista, processo_t *atual){
  processo_t *temp = lista;
  while(temp->proximo != NULL){
    if(temp->proximo == atual){
      temp->proximo = atual->proximo;
      atual->proximo = NULL;
    }else{
      temp = temp->proximo;
    }
  }
  temp->proximo = atual;
}


// Pega o terminal que esta bloqueado
int processos_pega_terminal_bloqueio(processo_t *self){
  return self->terminal_bloqueio;
}

// Pega o tipo do bloqueio
int processos_pega_tipo_bloqueio(processo_t *self){
  return self->tipo_bloqueio;
}

// Contabiliza o tempo em bloqueado e pronto
void processos_contabiliza_estatisticas(processo_t *lista){
  processo_t *temp = lista;
  while(temp != NULL){
    if(temp->estado == BLOQUEADO){
      temp->tempo_em_bloqueio++;
    }else if(temp->estado == PRONTO){
      temp->tempo_em_pronto++;
    }
    temp = temp->proximo;
  }
}

// Add 1 numérico ao tempo de execução do processo
void processos_add_tempo_execucao(processo_t *self){
  self->tempo_em_execucao = self->tempo_em_execucao + 1;
}

// Add 1 numérico ao tempo de bloqueio do processo
void processos_add_tempo_bloqueio(processo_t *self){
  self->tempo_em_bloqueio = self->tempo_em_bloqueio + 1;
}

// Atualiza os dados de um processo que já existe dentro da lista de processos
void processos_atualiza_dados_processo(processo_t *self, estado_t estado, cpu_estado_t *cpu){
  // Atualiza o numero de bloqueios e desbloqueios
  if(estado == BLOQUEADO && (self->estado == PRONTO || self->estado == EXECUCAO)){
    self->numero_bloqueios++;
  }else if(estado == PRONTO && self->estado == EXECUCAO){
    self->numero_preempcoes++;
  }else if(estado == EXECUCAO && self->estado == PRONTO){
    self->numero_execucoes++;
  }
  
  self->estado = estado;
  cpue_copia(cpu, self->cpue);
}

// Atualiza o estado de um processo pelo id, se for para BLOQUEADO armazena o tipo de bloqueio e o terminal referência
void processos_atualiza_estado(processo_t *lista, int id, estado_t estado, acesso_t tipo_bloqueio, int terminal_bloqueio){
  processo_t *temp = lista;
  while (temp != NULL && temp->id != id)
  {
    temp = temp->proximo;
  }
  if(temp != NULL){
    // Atualiza o numero de bloqueios
    if(estado == BLOQUEADO && (temp->estado == PRONTO || temp->estado == EXECUCAO)){
      temp->numero_bloqueios++;
    }else if(estado == PRONTO && temp->estado == EXECUCAO){
      temp->numero_preempcoes++;
    }else if(estado == EXECUCAO && temp->estado == PRONTO){
      temp->numero_execucoes++;
    }

    // Atualiza o estado do processo 
    temp->estado = estado;
    if(estado == BLOQUEADO){
      temp->tipo_bloqueio = tipo_bloqueio; 
      temp->terminal_bloqueio = terminal_bloqueio; 
    }
  }  
}


// Pega o numero de bloqueios do processo
int processos_pega_numero_bloqueios(processo_t *self){
  return self->numero_bloqueios;
}

// Pega o numero de desbloqueios do processo
int processos_pega_numero_desbloqueios(processo_t *self){
  return self->numero_desbloqueios;
}

// Pega o numero de preempcoes do processo
int processos_pega_numero_preempcoes(processo_t *self){
  return self->numero_preempcoes;
}

// Pega o tempo de execucão do processo
int processos_pega_tempo_em_execucao(processo_t *self){
  return self->tempo_em_execucao;
}

// Pega o tempo de execucão de bloqueio do processo
int processos_pega_tempo_em_bloqueio(processo_t *self){
  return self->tempo_em_bloqueio;
}

// Pega o tempo de execucão em pronto do processo
int processos_pega_tempo_em_pronto(processo_t *self){
  return self->tempo_em_pronto;
}

// Pega o tempo de retorno do processo
int processos_pega_tempo_de_retorno(processo_t *self, int tempo_final){
  return tempo_final - self->tempo_inicio;
}

// Pega a media de tempo de retorno do processo
float processos_pega_media_tempo_de_retorno(processo_t *self){
  if(self->numero_bloqueios == 0){
    return 0;
  }
  return (self->tempo_em_bloqueio + self->tempo_em_pronto) / self->numero_bloqueios;
}

// Pega o quantum do processo
int processos_pega_quantum(processo_t *self){
  if(self->id == 0){
    return 1;
  }
  return self->quantum;
}

// Seta o quantum do processo
void processos_set_quantum(processo_t *self, int quantum){
  self->quantum = quantum;
}


// Atualiza o estado de um processo, se for para BLOQUEADO armazena o tipo de bloqueio e o terminal referência
void processos_atualiza_estado_processo(processo_t *self, estado_t estado, acesso_t tipo_bloqueio, int terminal_bloqueio){
  // Atualiza o numero de bloqueios
  if(estado == BLOQUEADO && (self->estado == PRONTO || self->estado == EXECUCAO)){
    self->numero_bloqueios++;
  }else if(estado == PRONTO && self->estado == EXECUCAO){
    self->numero_preempcoes++;
  }else if(estado == EXECUCAO && self->estado == PRONTO){
    self->numero_execucoes++;
  }
  self->estado = estado; 
  if(estado == BLOQUEADO){
    self->tipo_bloqueio = tipo_bloqueio; 
    self->terminal_bloqueio = terminal_bloqueio; 
  }
}

// Verifica se existe um processo pelo id
bool processos_existe(processo_t *lista, int id){
  processo_t *temp = lista;
  while (temp != NULL && temp->id != id)
  {
    temp = temp->proximo;
  }
  if(temp != NULL){
    return true;
  }else{
    return false;
  }
}

// Pega o processo pelo id
processo_t *processos_pega_processo(processo_t *lista, int id){
  processo_t *temp = lista;
  while (temp != NULL && temp->id != id)
  {
    temp = temp->proximo;
  }
  if(temp != NULL){
    return temp;
  }
  return NULL;
}

// Pega o primeiro processo que estiver pronto sem contar o SO
processo_t *processos_pega_pronto(processo_t *lista){
  processo_t *temp = lista->proximo;
  while (temp != NULL && temp->estado != PRONTO) {
    temp = temp->proximo;
  }
  if(temp != NULL){
    return temp;
  }
  return NULL;
}

// Pega o processo que for menor sem contar o SO
processo_t *processos_pega_menor(processo_t *lista, int quantum){
  processo_t *temp = lista;

  // Variaveis que axiliarão na busca do menor
  float menor;
  float menorAuxiliar;
  processo_t *menor_processo = temp;

  // Percorre a lista
  while (temp != NULL) {
    temp = temp->proximo;
    if(temp != NULL){
      if(temp->estado == PRONTO){
        if(temp->numero_execucoes == 1){
          menorAuxiliar = quantum;
        }else{
          menorAuxiliar = temp->tempo_em_execucao / temp->numero_execucoes;
        }
        if(menor_processo == lista){
          menor = menorAuxiliar;
          menor_processo = temp;
        }else if(menorAuxiliar < menor){
          menor = menorAuxiliar;
          menor_processo = temp;
        }
      }
    }
  }
  if(menor_processo == lista){
    return NULL;
  }
  return menor_processo;
}


// Pega o processo que estiver em execução
processo_t *processos_pega_execucao(processo_t *lista){
  processo_t *temp = lista;
  while (temp != NULL && temp->estado != EXECUCAO)
  {
    temp = temp->proximo;
  }
  if(temp != NULL){
    return temp;
  }
  return NULL;
}

// Pega a cpu de um processo
cpu_estado_t *processos_pega_cpue(processo_t *self){
  return self->cpue;
}

// Pega o id de um processo
int processos_pega_id(processo_t *self){
  return self->id;
}

// Pega o estado de um processo
estado_t processos_pega_estado(processo_t *self){
  return self->estado;
}

// Pega fim memoria processo
int processos_pega_fim(processo_t *self){
  return self->fim_memoria;
}

// Pega inicio memoria processo
int processos_pega_inicio(processo_t *self){
  return self->inicio_memoria;
}

// Destroi todos os processos
void processos_destroi(processo_t *lista){
  processo_t *temp = lista;
  while (temp != NULL)
  {
    processo_t *aux = temp->proximo;
    free(temp->cpue);
    free(temp);
    temp = aux;
  }
  return;
}

