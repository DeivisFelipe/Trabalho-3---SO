#ifndef MMU_H
#define MMU_H

// simulador do gerenciador de memória
// recebe pedidos de acesso à memória virtual, traduz endereços e,
//   se bem sucedido, repassa à memória física
// faz a tradução usando uma tabela de páginas

#include "err.h"
#include "mem.h"
#include "tab_pag.h"

// tipo opaco que representa o gerenciador de memória
typedef struct mmu_t mmu_t;

// tipo opaco que representa um quadro
typedef struct quadro_t quadro_t;

// cria um gerenciador de memória para gerenciar acessos à memória
//   representada por 'mem'
// deve receber uma tabela de páginas para poder realizar a conversão
//   de endereços, senão simplesmente repassará os pedidos à mem.
// retorna um ponteiro para um descritor, que deverá ser usado em todas
//   as operações
// retorna NULL em caso de erro
mmu_t *mmu_cria(mem_t *mem);

// destrói um gerenciador de memória
// nenhuma outra operação pode ser realizada no gerenciador após
//   esta chamada
void mmu_destroi(mmu_t *self);

// muda a tabela de páginas a usar para a tradução de endereços
//   virtuais em físicos nos próximos acessos à memória
// se 'tab_pag' for NULL, não será feita tradução, os endereços
//   virtuais recebidos serão repassados sem alteração à memória
void mmu_usa_tab_pag(mmu_t *self, tab_pag_t *tab_pag);

// coloca na posição apontada por 'pvalor' o valor no endereço
//   virtual 'endereco'
// retorna erro retornado pela tabela de páginas se o endereço não
//   puder ser traduzido para um endereço físico ou o erro retornado
//   pela memória se o acesso ao endereço não puder ser feito, ou ERR_OK
err_t mmu_le(mmu_t *self, int endereco, int *pvalor);

// coloca 'valor' no endereço virtual 'endereco'
// retorna erro retornado pela tabela de páginas se o endereço não
//   puder ser traduzido para um endereço físico ou o erro retornado
//   pela memória se o acesso ao endereço não puder ser feito, ou ERR_OK
err_t mmu_escreve(mmu_t *self, int endereco, int valor);

// retorna o último endereço virtual que a MMU traduziu (ou tentou traduzir)
// função usada pelo SO para obter o endereço que causou falha de página
int mmu_ultimo_endereco(mmu_t *self);


// Gerenciamento de quadros

// Pega um quadro livre
quadro_t *mmu_retira_quadro_livre(mmu_t *self);

// Pega um guardo ocupado
quadro_t *mmu_retira_quadro_ocupado(mmu_t *self);

// Imprime os quadros livres
void mmu_imprime_quadros_livres(mmu_t *self);

// Imprime os quadros ocupados
void mmu_imprime_quadros_ocupados(mmu_t *self);

// Imprime todos os quadros
void mmu_imprime_quadros(mmu_t *self);

// cria um quadro livre
void mmu_insere_quadro_livre(mmu_t *self, int id, int endereco_principal_inicio, int endereco_principal_fim, int endereco_secundario_inicio, int endereco_secundario_fim);

// cria um quadro ocupado
void mmu_insere_quadro_ocupado(mmu_t *self, int id, int endereco_principal_inicio, int endereco_principal_fim, int endereco_secundario_inicio, int endereco_secundario_fim);

// inicializa os quadros livres
void mmu_inicia_quadros_livres(mmu_t *self, int tamanho_quadro);

#endif // MMU_H