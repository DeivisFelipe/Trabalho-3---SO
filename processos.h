#ifndef PROCESSOS_H
#define PROCESSOS_h

#include "mem.h"
#include "es.h"
#include "cpu_estado.h"
#include "tab_pag.h"
#include <stdbool.h>

typedef enum {
  EXECUCAO,     // Processo em execucao
  PRONTO,       // Processo pronto
  BLOQUEADO,    // Processo bloqueado
} estado_t;

typedef struct processo_t processo_t;
typedef struct historico_t historico_t;

processo_t* processos_cria(int id, estado_t estado , mem_t *mem, int inicio_memoria, int fim_memoria, cpu_estado_t *cpu, int tempo_inicio, int tamanho_pagina, int numero_de_paginas);

// Destroi todos os processos
void processos_destroi(processo_t *lista);

// Verifica desbloqueio de processo
void processos_desbloqueia(processo_t *lista, es_t *estrada_saida);

// Insere um novo processo no final da fila
processo_t *processos_insere(processo_t *lista, int id, estado_t estado, int inicio_memoria, int fim_memoria, cpu_estado_t *cpu, int tempo_inicio, int quantum, int tamanho_pagina, int numero_de_paginas);

// Funcao que printa todos os processos
void processos_printa(processo_t *lista);

// Retorna a tabela de paginação do processo atual
tab_pag_t *processos_tabela_de_pag(processo_t *self);

// Remove um processo
void processos_remove(processo_t *lista, processo_t *atual);

// Muda o estado de um processo pelo id
void processos_atualiza_estado(processo_t *lista, int id, estado_t estado, acesso_t tipo_bloqueio, int terminal_bloqueio);

// Muda o estado de um processo
void processos_atualiza_estado_processo(processo_t *lista, estado_t estado, acesso_t tipo_bloqueio, int terminal_bloqueio);

// Atualiza todas as informações de um processo pelo id
void processos_atualiza_dados(processo_t *lista, int id, estado_t estado, cpu_estado_t *cpu);

// Atualiza os dados de um processo que já existe dentro da lista de processos
void processos_atualiza_dados_processo(processo_t *selt, estado_t estado, cpu_estado_t *cpu);

// Contabiliza o tempo em bloqueado e pronto
void processos_contabiliza_estatisticas(processo_t *lista);

// Bota o processo no fim da fila
void processos_bota_fim(processo_t *lista, processo_t *atual);

// Pega o tempo de retorno do processo
int processos_pega_tempo_de_retorno(processo_t *self, int tempo_final);

// Pega o processo pelo id
processo_t *processos_pega_processo(processo_t *lista, int id);

// Pega processo em execução
processo_t *processos_pega_execucao(processo_t *lista);

// Pega processo que esta pronto
processo_t *processos_pega_pronto(processo_t *lista);

// Pega o processo que for menor sem contar o SO
processo_t *processos_pega_menor(processo_t *lista, int quantum);

// Pega o terminal que esta bloqueado
int processos_pega_terminal_bloqueio(processo_t *self);

// Pega o tipo do bloqueio
int processos_pega_tipo_bloqueio(processo_t *self);

// Verifica se existe o processo com o id
bool processos_existe(processo_t *lista, int id);

// Pega a cpu do processo
cpu_estado_t *processos_pega_cpue(processo_t *self);

// Pega o id do processo
int processos_pega_id(processo_t *self);

// Pega fim memoria processo
int processos_pega_fim(processo_t *self);

// Pega inicio memoria processo
int processos_pega_inicio(processo_t *self);

// Add 1 numérico ao tempo de execução do processo
void processos_add_tempo_execucao(processo_t *self);

// Add 1 numérico ao tempo de bloqueio do processo
void processos_add_tempo_bloqueio(processo_t *self);

// Pega o numero de bloqueios do processo
int processos_pega_numero_bloqueios(processo_t *self);

// Pega o numero de desbloqueios do processo
int processos_pega_numero_desbloqueios(processo_t *self);

// Pega o numero de preempcoes do processo
int processos_pega_numero_preempcoes(processo_t *self);

// Pega o tempo de execucão do processo
int processos_pega_tempo_em_execucao(processo_t *self);

// Pega o tempo de execucão de bloqueio do processo
int processos_pega_tempo_em_bloqueio(processo_t *self);

// Pega o tempo de execucão em pronto do processo
int processos_pega_tempo_em_pronto(processo_t *self);

// Pega a media de tempo de retorno do processo
float processos_pega_media_tempo_de_retorno(processo_t *self);

// Pega o quantum do processo
int processos_pega_quantum(processo_t *self);

// Seta o quantum do processo
void processos_set_quantum(processo_t *self, int quantum);

// Pega o estado do processo
estado_t processos_pega_estado(processo_t *self);
#endif