#include "mem.h"
#include "tela.h"
#include "instr.h"
#include <stdlib.h>

// tipo de dados opaco para representar uma região de memória
struct mem_t {
  int tam;
  int *conteudo;
  int utilizado;
  int inicio_executando;
  int fim_executando;
  int tipo; // 0 = memoria principal, 1 = memoria secundaria
};

mem_t *mem_cria(int tam, int tipo) {
  mem_t *self;
  self = malloc(sizeof(*self));
  if (self != NULL) {
    self->tam = tam;
    self->utilizado = 0;
    self->tipo = tipo;
    self->conteudo = malloc(tam * sizeof(*(self->conteudo)));
    if (self->conteudo == NULL) {
      free(self);
      self = NULL;
    }else{
      // Inizializa a memoria com 0
      int i;
      for(i = 0; i < tam; i++){
        self->conteudo[i] = 0;
      }
    }	
  }
  return self;
}

void mem_destroi(mem_t *self)
{
  if (self != NULL) {
    if (self->conteudo != NULL) {
      free(self->conteudo);
    }
    free(self);
  }
}

void mem_copia(mem_t *self, mem_t *outro)
{
  *outro = *self;
}

int mem_tam(mem_t *self)
{
  return self->tam;
}

// Printa a memoria de um parte
void mem_printa(mem_t *self, mem_t *compara){
  int i;
  int pc;
  if(self->tipo == 0){
    t_printf("MEMORIA PRINCIPAL");
    for (i = 0/*self->inicio_executando*/, pc = 0; i < 12; i++, pc++){
      t_printf("mem[%2d] = %4d - ins = %s", i, self->conteudo[i], instr_nome(self->conteudo[i]));
    }
  } else {
    t_printf("INICIO: %d", self->inicio_executando);
    t_printf("MEMORIA SECUNDARIA");
    if(compara == NULL){
      for (i = self->inicio_executando, pc = 0; i < self->fim_executando; i++, pc++){
        t_printf("mem[%2d] = %4d - ins = %s", i, self->conteudo[i], instr_nome(self->conteudo[i]));
      }
    }else{
      for (i = self->inicio_executando, pc = 0; i < self->fim_executando; i++, pc++){
        t_printf("mem[%2d] = %4d - ins = %s \t# mem[%2d] = %4d - ins = %s ", i, self->conteudo[i], instr_nome(self->conteudo[i]), i, compara->conteudo[i], instr_nome(compara->conteudo[i]));
      }
    }
    t_printf("INICIO: %d", self->inicio_executando);
    t_printf("FIM: %d", self->fim_executando);
  }
}

// Muda o valor de inicio da memoria do programa do processo
void mem_muda_inicio_executando(mem_t *self, int pos){
  self->inicio_executando = pos;
}

// Muda o valor de fim da memoria do programa do processo
void mem_muda_fim_executando(mem_t *self, int pos){
  self->fim_executando = pos;
}

// Pega a tamanho de memoria utilizada
int mem_utilizado(mem_t *self){
  return self->utilizado;
}

// Retorna o ponteiro do conteudo
int *mem_conteudo(mem_t *self){
  return self->conteudo;
}

// Muda o valor de memoria utilizada
void mem_muda_utilizado(mem_t *self, int utilizado){
  self->utilizado = utilizado;
}

// função auxiliar, verifica se endereço é válido
static err_t verif_permissao(mem_t *self, int endereco)
{
  if (endereco < 0 || endereco >= self->tam/* || endereco < self->inicio_executando || endereco >= self->fim_executando */) {
    return ERR_END_INV;
  }
  return ERR_OK;
}

err_t mem_le(mem_t *self, int endereco, int *pvalor)
{
  // Pega o PC e soma com o valor inicial da memoria do programa
  if(self->tipo == 0){
    endereco = endereco;
  } else {
    // mem_secundaria
    endereco = self->inicio_executando + endereco;
  }
  err_t err = verif_permissao(self, endereco);
  if (err == ERR_OK) {
    *pvalor = self->conteudo[endereco];
  }
  return err;
}

err_t mem_escreve(mem_t *self, int endereco, int valor, bool salva_quadro)
{
  if(self->tipo == 0){
    endereco = endereco;
  } else {
    // mem_secundaria
    if(salva_quadro){
      endereco = endereco;
    }else{
      endereco = self->inicio_executando + endereco;
    }
  }
  err_t err = verif_permissao(self, endereco);
  if (err == ERR_OK) {
    self->conteudo[endereco] = valor;
  }
  return err;
}

