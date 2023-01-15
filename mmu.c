#include "mmu.h"
#include "tab_pag.h"
#include "tela.h"
#include <stdlib.h>

// tipo de dados opaco para representar o controlador de memória
struct mmu_t {
  mem_t *mem;          // a memória física
  tab_pag_t *tab_pag;  // a tabela de páginas
  int ultimo_endereco; // o último endereço virtual traduzido pela MMU
  // Controle de quadros
  quadro_t *quadros_livres;
  quadro_t *quadros_ocupados;
  // Tipo de escalonador
  int tipo_escalonador;
  // Tamanho do quadro
  int tamanho_quadro;
};

struct quadro_t {
  int id;
  int endereco_principal_inicio;
  int endereco_principal_fim;
  int endereco_secundario_inicio;
  int endereco_secundario_fim;
  // Referencia sobre a tabela de páginas
  tab_pag_t *tab_pag;
  int pagina;
  quadro_t *proximo;
};


mmu_t *mmu_cria(mem_t *mem)
{
  mmu_t *self;
  self = malloc(sizeof(*self));
  if (self != NULL) {
    self->mem = mem;
    self->tab_pag = NULL;
    self->quadros_ocupados = NULL;
    self->quadros_livres = NULL;
  }
  return self;
}

void mmu_seta_tipo_escalonador(mmu_t *self, int tipo_escalonador){
  self->tipo_escalonador = tipo_escalonador;
}

void mmu_seta_tamanho_quadro(mmu_t *self, int tamanho_quadro){
  self->tamanho_quadro = tamanho_quadro;
}

void mmu_insere_quadro_livre_novo(mmu_t *self, int id, int endereco_principal_inicio, int endereco_principal_fim) {
  quadro_t *quadro = malloc(sizeof(*quadro));
  quadro->id = id;
  quadro->endereco_principal_inicio = endereco_principal_inicio;
  quadro->endereco_principal_fim = endereco_principal_fim;
  quadro->endereco_secundario_inicio = 0;
  quadro->endereco_secundario_fim = 0;
  quadro->proximo = self->quadros_livres;
  quadro->tab_pag = NULL;
  quadro->pagina = -1;
  self->quadros_livres = quadro;
}

void mmu_insere_quadro_livre(mmu_t *self, quadro_t *quadro) {
  quadro->proximo = self->quadros_livres;
  quadro->tab_pag = NULL;
  quadro->pagina = -1;
  quadro->endereco_secundario_inicio = 0;
  quadro->endereco_secundario_fim = 0;
  self->quadros_livres = quadro;
}

void mmu_insere_quadro_ocupado(mmu_t *self, quadro_t *quadro, tab_pag_t *tab_pag, int pagina, int endereco_secundario_inicio, int endereco_secundario_fim) {
  quadro->proximo = self->quadros_ocupados;
  quadro->tab_pag = tab_pag;
  quadro->pagina = pagina;
  quadro->endereco_secundario_inicio = endereco_secundario_inicio;
  quadro->endereco_secundario_fim = endereco_secundario_fim;
  self->quadros_ocupados = quadro;
}


void mmu_inicia_quadros_livres(mmu_t *self, int tamanho_quadro) {
  int tamanha_memoria = mem_tam(self->mem);
  int numero_quadros = tamanha_memoria / tamanho_quadro;
  int i;
  for (i = 0; i < numero_quadros; i++) {
    mmu_insere_quadro_livre_novo(self, i, i * tamanho_quadro, (i + 1) * tamanho_quadro - 1);
  }
}

void mmu_imprime_quadros_livres(mmu_t *self) {
  quadro_t *quadro = self->quadros_livres;
  while (quadro != NULL) {
    t_printf("Quadro L %.4d: principal= %.4d - %.4d # secundaria= %.4d - %.4d", quadro->id, quadro->endereco_principal_inicio, quadro->endereco_principal_fim, quadro->endereco_secundario_inicio, quadro->endereco_secundario_fim);
    quadro = quadro->proximo;
  }
}

void mmu_imprime_quadros_ocupados(mmu_t *self) {
  quadro_t *quadro = self->quadros_ocupados;
  while (quadro != NULL) {
    t_printf("Quadro O %.4d: principal= %.4d - %.4d # secundaria= %.4d - %.4d", quadro->id, quadro->endereco_principal_inicio, quadro->endereco_principal_fim, quadro->endereco_secundario_inicio, quadro->endereco_secundario_fim);
    quadro = quadro->proximo;
  }
}

void mmu_imprime_quadros(mmu_t *self) {
  mmu_imprime_quadros_livres(self);
  mmu_imprime_quadros_ocupados(self);
}

void mmu_imprime_memoria_quadro(mmu_t *self, quadro_t *quadro) {
  t_printf("Memoria do quadro: %d", quadro->id);
  for(int i = quadro->endereco_principal_inicio; i <= quadro->endereco_principal_fim; i++) {
    int valor;
    mem_le(self->mem, i, &valor);
    t_printf("Memoria: %.4d = %d", i, valor);
  }
}

void mmu_salva_memoria_secundaria(mmu_t *mmu, quadro_t *quadro, mem_t *mem_secundaria){
  //t_printf("Inicio %d", quadro->endereco_principal_inicio);
  //t_printf("Inicio Secundaria %d", quadro->endereco_secundario_inicio);
  int valor;
  for(int i = quadro->endereco_principal_inicio; i <= quadro->endereco_principal_fim; i++) {
    mem_le(mmu->mem, i, &valor);
    int posicao_secundaria = quadro->endereco_secundario_inicio + i - quadro->endereco_principal_inicio;
    mem_escreve(mem_secundaria, posicao_secundaria, valor, true);
  }

  // Invalida pagina antiga
  tab_pag_t *tab_pag = quadro->tab_pag;
  int pagina = quadro->pagina;
  tab_pag_muda_valida(tab_pag, pagina, 0);
}

quadro_t *mmu_retira_quadro_livre(mmu_t *self) {
  quadro_t *quadro = self->quadros_livres;
  if(quadro == NULL) {
    return NULL;
  }
  self->quadros_livres = quadro->proximo;
  return quadro;
}

quadro_t *mmu_retira_quadro_ocupado(mmu_t *self) {
  if(self->tipo_escalonador == 1 || self->tipo_escalonador == 2){
    quadro_t *quadro = self->quadros_ocupados;
    if(quadro == NULL) {
      return NULL;
    }
    if(quadro->proximo == NULL){
      self->quadros_ocupados = NULL;
    }
    while (quadro->proximo != NULL)
    {
      quadro_t *aux = quadro;
      quadro = aux->proximo;
      if(quadro->proximo == NULL){
        aux->proximo = NULL;
      }
    }
    t_printf("Quadro retirado: %d", quadro->id);
    return quadro;
  }
  return NULL;
}

void mmu_destroi(mmu_t *self)
{
  if (self != NULL) {
    for(quadro_t *quadro = self->quadros_livres; quadro != NULL; ) {
      quadro_t *aux = quadro;
      quadro = quadro->proximo;
      free(aux);
    }
    for(quadro_t *quadro = self->quadros_ocupados; quadro != NULL; ) {
      quadro_t *aux = quadro;
      quadro = quadro->proximo;
      free(aux);
    }
    free(self);
  }
}

void mmu_usa_tab_pag(mmu_t *self, tab_pag_t *tab_pag)
{
  self->tab_pag = tab_pag;
}

// função auxiliar, traduz um endereço virtual em físico
static err_t traduz_endereco(mmu_t *self, int end_v, int *end_f)
{
  self->ultimo_endereco = end_v;
  // se não tem tabela de páginas, não traduz
  if (self->tab_pag == NULL) {
    *end_f = end_v;
    return ERR_OK;
  }
  return tab_pag_traduz(self->tab_pag, end_v, end_f);
}

err_t mmu_le(mmu_t *self, int endereco, int *pvalor)
{
  int end_fis;
  err_t err = traduz_endereco(self, endereco, &end_fis);
  if (err != ERR_OK) {
    return err;
  }

  if(self->tipo_escalonador == 2){
    int id_quadro = end_fis / self->tamanho_quadro;
    quadro_t *quadro = mmu_retorna_quadro(self, id_quadro);
    if(quadro != NULL){
      mmu_coloca_quadro(self, quadro);
    }
  }
  
  return mem_le(self->mem, end_fis, pvalor);
}

err_t mmu_escreve(mmu_t *self, int endereco, int valor)
{
  int end_fis;
  err_t err = traduz_endereco(self, endereco, &end_fis);
  if (err != ERR_OK) {
    return err;
  }

  if(self->tipo_escalonador == 2){
    int id_quadro = end_fis / self->tamanho_quadro;
    quadro_t *quadro = mmu_retorna_quadro(self, id_quadro);
    if(quadro != NULL){
      mmu_coloca_quadro(self, quadro);
    }
  }

  return mem_escreve(self->mem, end_fis, valor, false);
}

int mmu_ultimo_endereco(mmu_t *self)
{
  return self->ultimo_endereco;
}

int mmu_pega_id_quadro(quadro_t *quadro){
  return quadro->id;
}

// Pega o quadro, tira ele da possição atual e coloca na frente da lista
void mmu_coloca_quadro(mmu_t *self, quadro_t *quadro){
  // Tira o quadro da possição atual e bota na frente da lista
  quadro_t *aux = self->quadros_ocupados;
  if(aux == quadro){
    return;
  } else {
    while (aux->proximo != quadro)
    {
      aux = aux->proximo;
    }
    aux->proximo = quadro->proximo;
    aux = self->quadros_ocupados;
    quadro->proximo = aux;
    self->quadros_ocupados = quadro;
  }
}

// Retorna o quadro com o id passado
quadro_t *mmu_retorna_quadro(mmu_t *self, int idQuadro){
  quadro_t *quadro = self->quadros_ocupados;
  while (quadro != NULL) {
    if(quadro->id == idQuadro){
      return quadro;
    }
    quadro = quadro->proximo;
  }
  return NULL;
}

// Remove o quadro dos ocupados pelo id
quadro_t *mmu_remove_quadro_ocupado(mmu_t *self, int idQuadro){
  quadro_t *quadro = self->quadros_ocupados;
  quadro_t *quadro_anterior = NULL;
  while (quadro != NULL) {
    if(quadro->id == idQuadro){
      if(quadro_anterior == NULL){
        self->quadros_ocupados = quadro->proximo;
      }else{
        quadro_anterior->proximo = quadro->proximo;
      }
      return quadro;
    }
    quadro_anterior = quadro;
    quadro = quadro->proximo;
  }
  return NULL;
}