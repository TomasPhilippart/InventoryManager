/*
    * Ficheiro: stock_manager.c
    * Autor: Tomas Philippart, aluno 95683
    * Descricao: Programa
*/

#include <stdio.h>
#include <strings.h>

/* Retirar comentario para ativar funcao 
   de debug, executada pelo carater 'D' */
#define DEBUG

#define MAX_DESCRICAO 64
#define MAX_PRODUTOS 10000
#define MAX_ENCOMENDAS 500
#define MAX_PESO 200

/* estes 2 defines sao usados para os parametros da 
   matriz 2x2 produtos, na estrutura Encomenda */
#define IDP 0
#define QTD 1

/* ============================ Produto ============================*/

typedef struct Produto {
    /*
    Cada produto e' caraterizado por:
       > Identificador (int idp) - [0, 9 999]
       > Descricao (int descricao) - max 63 carateres
       > Preco (int preco) - (> 0)
       > Peso (int peso) - (> 0)
       > Quantidade em stock (int stock) - (>= 0) */

    int idp, preco, peso, stock;
    char descricao[MAX_DESCRICAO];
} Produto;


/* ============================ Encomenda ============================*/
/* Uma encomenda e um conjunto de produtos */
typedef struct Encomenda {

   /* Array produtos que guarda os produtos na encomenda 
      na forma [[idp_0, qtd_0], [idp_1, qtd_1], ...] */
   int produtos[MAX_PRODUTOS][2];

   int peso, num_produtos_distintos;
} Encomenda;


/* ============================ Armazem ============================*/
/* O armazem contem o conjunto de produtos e o conjunto de encomendas */

struct Armazem {

    /* Tabela com capacidade para armazenar todos os produtos */
    Produto produtos[MAX_PRODUTOS];
    /* Variavel para manter em conta o numero de produtos */
    int num_produtos;

    /* Tabela com capacidade para armazenar todas as encomendas */
    Encomenda encomendas[MAX_ENCOMENDAS];
    /* Variavel para manter em conta o numero de encomendas */
    int num_encomendas;    
} armazem = {0};


/* Criterio de ordenacao dos produtos */
typedef enum {preco, descricao} criterio;

/* Prototipo de funcoes */
#ifdef DEBUG
    void debug();
#endif
void novo_produto();
void add_stock();
void nova_encomenda();
void add_prod_encomenda();
void novo_prod_encomenda(int ide, int idp, int qtd);
void remove_stock();
void remove_prod_encomenda();
void remove_produto_encomenda_aux(int ide, int idp);
void calcula_custo_encomenda();
void set_preco_prod();
void lista_descricao_qtd_prod_encomenda();
void lista_ide_max_produto();
void lista_produtos_por_preco();
void lista_produtos_alfabeticamente_descricao();

/* ============================ MAIN ============================ */
int main() {

    int ch;

    while (1) {        
        /* le os comandos e executa-os */
        switch(ch = getchar()) {

            #ifdef DEBUG
                case 'D':
                    debug();
                    break;
            #endif

            case 'a':
                novo_produto();
                break;

            case 'q':
                add_stock();
                break;
            
            case 'N':
                nova_encomenda();
                break;
            
            case 'A':
                add_prod_encomenda();
                break;
            
            case 'r':
                remove_stock();
                break;
            
            case 'R':
                remove_prod_encomenda();
                break;
            
            case 'C':
                calcula_custo_encomenda();
                break;
            
            case 'p':
                set_preco_prod();
                break;

            case 'E':
                lista_descricao_qtd_prod_encomenda();
                break;
            
            case 'm':
                lista_ide_max_produto();
                break;
            
            case 'l':
                lista_produtos_por_preco();
                break;
            
            case 'L':
                lista_produtos_alfabeticamente_descricao();
                break;
            
            case 'x':
                /* Termina o programa. */
                return 0;

        }
    }
}



/* ============================ Funcoes ============================*/

/* Adicona um novo produto ao sistema. */
void novo_produto() {

    int idp = armazem.num_produtos;
    scanf(" %[^:]:%d:%d:%d", armazem.produtos[idp].descricao, 
                             &armazem.produtos[idp].preco,
                             &armazem.produtos[idp].peso, 
                             &armazem.produtos[idp].stock);
    
    armazem.produtos[idp].idp = idp;
    printf("Novo produto %d.\n", idp);
    armazem.num_produtos++;
}

/* Adiciona stock a um produto existente no sistema. */
void add_stock() {

    int idp, qtd;
    scanf(" %d:%d", &idp, &qtd);  

    if (idp >= armazem.num_produtos) {
        printf("Impossivel adicionar produto %d ao stock. Produto inexistente.\n", idp);
    }
    armazem.produtos[idp].stock += qtd;
}


/* Cria uma nova encomenda. */
void nova_encomenda() {

    int ide = armazem.num_encomendas;

    armazem.encomendas[ide].peso = 0;
    armazem.encomendas[ide].num_produtos_distintos = 0;

    printf("Nova encomenda %d.\n", armazem.num_encomendas);
    armazem.num_encomendas++;
}


/* Adiciona um produto a uma encomenda. Se o produto
   ja existir na encomenda, adiciona quantidade. */
void add_prod_encomenda() {

   int ide, idp, qtd, i;

   scanf(" %d:%d:%d", &ide, &idp, &qtd);

    /* Verificar se a encomenda existe */
    if (ide >= armazem.num_encomendas) {
        printf("Impossivel adicionar produto %d a encomenda %d. Encomenda inexistente.\n", idp, ide);
        return;
    }

    /* Verificar se o produto existe */
    if (idp >= armazem.num_produtos) {
        printf("Impossivel adicionar produto %d a encomenda %d. Produto inexistente.\n", idp, ide);
        return;
    }

    /* Verificar se a quantidade em stock e suficiente */
    if (qtd > armazem.produtos[idp].stock) {
        printf("Impossivel adicionar produto %d a encomenda %d. Quantidade em stock insuficiente.\n", idp, ide);
        return;
    }

    /* Verificar se o peso da encomenda excede o maximo de 200 */
    if (armazem.encomendas[ide].peso + (armazem.produtos[idp].peso * qtd) > MAX_PESO) {
        printf("Impossivel adicionar produto %d a encomenda %d. Peso da encomenda excede o maximo de 200.\n", idp, ide);
        return;
    }

    /* procurar pelo produto na encomenda */
    for (i = 0; i < armazem.encomendas[ide].num_produtos_distintos; i++) {

        /* se o produto ja existir na encomenda, adiciona quantidade */
        if (armazem.encomendas[ide].produtos[i][IDP] == idp) {
            armazem.encomendas[ide].produtos[i][QTD] += qtd;

            /* subtrai a quantidade do stock */
            armazem.produtos[idp].stock -= qtd;

            /* aumenta peso*/ 
            armazem.encomendas[ide].peso += armazem.produtos[idp].peso * qtd;
            return;
        } 
    }

    novo_prod_encomenda(ide, idp,qtd);
}


/* Funcao auxiliar que CRIA um produto e ADICIONA-o a uma encomenda */ 
void novo_prod_encomenda(int ide, int idp, int qtd) {
    /* variavel auxiliar, para nao repetir o comboio */
    int num_produtos = armazem.encomendas[ide].num_produtos_distintos;

    /* adiciona o produto a encomenda, substituindo a quantidade e o identificador */
    armazem.encomendas[ide].produtos[num_produtos][QTD] = qtd;
    armazem.encomendas[ide].produtos[num_produtos][IDP] = idp;
    
    /* aumenta peso*/ 
    armazem.encomendas[ide].peso += armazem.produtos[idp].peso * qtd;

    /* incrementar o numero de produtos distintos*/
    armazem.encomendas[ide].num_produtos_distintos++;

    /* subtrai a quantidade do stock */
    armazem.produtos[idp].stock -= qtd;
}


/* Remove stock a um produto existente */
void remove_stock() {
    int idp, qtd_a_remover;
    scanf(" %d:%d", &idp, &qtd_a_remover);

    /* Verificar se o produto existe */
    if (idp >= armazem.num_produtos) {
        printf("Impossivel remover stock do produto %d. Produto inexistente.\n", idp);
        return;
    }

    /* Verificar se a quantidade em stock e suficiente */
    if (qtd_a_remover > armazem.produtos[idp].stock) {
        printf("Impossivel remover stock do produto %d. Quantidade insuficiente.\n", idp);
        return;
    }

    armazem.produtos[idp].stock -= qtd_a_remover;
}


/* Remove um produto de uma encomenda */
void remove_prod_encomenda() {
    int ide, idp, i;


    scanf(" %d:%d", &ide, &idp);

    /* Verificar se a encomenda existe */
    if (ide >= armazem.num_encomendas) {
        printf("Impossivel remover produto %d a encomenda %d. Encomenda inexistente.\n", idp, ide);
        return;
    }

    /* Verificar se o produto existe */
    if (idp >= armazem.num_produtos) {
        printf("Impossivel remover produto %d a encomenda %d. Produto inexistente.\n", idp, ide);
        return;
    }


    /* adiciona stock ao produto a remover */
    armazem.produtos[idp].stock += armazem.encomendas[ide].produtos[idp][QTD];

    /* remove peso da encomenda */
    armazem.encomendas[ide].peso -= armazem.produtos[idp].peso * 
                            armazem.encomendas[ide].produtos[idp][QTD];

    /* percorrer o tabela ate encontrar o produto a remover */
    for (i = 0; i < armazem.encomendas[ide].num_produtos_distintos; i++) {
        if (armazem.encomendas[ide].produtos[i][IDP] == idp) {
            remove_produto_encomenda_aux(ide, idp);

            armazem.encomendas[ide].num_produtos_distintos--;
            return;
        }
    }
}

/* Funcao auxiliar para "remover" o produto de uma encomenda*/
void remove_produto_encomenda_aux(int ide, int idp) {
    int j;
    /* encontrado o produto a remover, vou passar os produtos "depois" dele 
        para a "esquerda" */
    for (j = idp; j < armazem.encomendas[ide].num_produtos_distintos; j++) {
        /* "empurrar" os produtos para tras, substituindo o produto a remover */     
            armazem.encomendas[ide].produtos[j][IDP] = armazem.encomendas[ide].produtos[j + 1][IDP];
            armazem.encomendas[ide].produtos[j][QTD] = armazem.encomendas[ide].produtos[j + 1][QTD];
            }
}

/* Calcula custo da encomenda */
void calcula_custo_encomenda() {
    int i, qtd, ide, idp, custo = 0;

    scanf(" %d", &ide);

    /* Verificar se a encomenda existe */ 
    if (ide >= armazem.num_encomendas) {
        printf("Impossivel calcular custo da encomenda %d. Encomenda inexistente.\n", ide);
        return;
    }

    for (i = 0; i < armazem.encomendas[ide].num_produtos_distintos; i++) {
        idp = armazem.encomendas[ide].produtos[i][IDP];
        qtd = armazem.encomendas[ide].produtos[i][QTD];
        custo += (armazem.produtos[idp].preco * qtd);
    }

    printf("Custo da encomenda %d %d\n", ide, custo);
}


/* Altera o preco de um produto existente no sistema */
void set_preco_prod() {
    int idp, preco;

    scanf(" %d:%d", &idp, &preco);

    /* Verificar se o produto existe */
    if (idp >= armazem.num_produtos) {
        printf("Impossivel alterar preco do produto %d. Produto inexistente.", idp);
        return;
    }

    armazem.produtos[idp].preco = preco;
}


/* lista a descrição e a quantidade de um produto numa encomenda */
void lista_descricao_qtd_prod_encomenda() {
    int ide, idp;

    scanf(" %d:%d", &ide, &idp);

    /* Verificar se a encomenda existe */ 
    if (ide >= armazem.num_encomendas) {
        printf("Impossivel listar encomenda %d. Encomenda inexistente.\n", ide);
        return;
    }

    /* Verificar se o produto existe */
    if (idp >= armazem.num_produtos) {
        printf("Impossivel listar produto %d. Produto inexistente.", idp);
        return;
    }

    printf("%s %d", armazem.produtos[idp].descricao, armazem.encomendas[ide].produtos[idp][QTD]);
}


/* lista o identificador da encomenda em que o produto dado ocorre mais vezes */
void lista_ide_max_produto() {

    int i, j, idp, ide_max_produto, max_quantidade = 0;

    scanf(" %d", &idp);

    /* Verificar se o produto existe */
    if (idp >= armazem.num_produtos) {
        printf("Impossivel listar maximo do produto %d. Produto inexistente.\n", idp);
        return;
    }
    /* iteracao por todas as encomendas */
    for (j = 0; j < armazem.num_encomendas; j++) {

        /* iteracao por todos os produtos da encomenda j */
        for (i = 0; i < armazem.encomendas[j].num_produtos_distintos; i++) {
            /* se encontrarmos o produto numa dada encomenda*/
            if (armazem.encomendas[j].produtos[i][IDP] == idp) {
                /* se a quantidade for maior do que a quantidade do maior produto,
                esta sera a encomenda onded o produto ocorre mais vezes */
                if (armazem.encomendas[j].produtos[i][QTD] > max_quantidade) {
                    ide_max_produto = j;
                    max_quantidade = armazem.encomendas[i].produtos[i][QTD];
                }
            }
        }   

    }

    /* se nao houver encomendas */
    if (max_quantidade == 0) return;
    

    printf("Maximo produto %d %d %d.\n", idp, ide_max_produto, max_quantidade);
}


void lista_produtos_por_preco() {
    
}

void lista_produtos_alfabeticamente_descricao() {
    
}

static void troca(Produto p[], int i, int j) {
    Produto temp = p[i];
    p[i] = p[j];
    p[j] = temp;
}

int inferior(Produto a, Produto b, criterio c) {
    switch(c) {

        case preco:
            /* se o preco for igual, ordena por idp crescente */
            return ((a.preco < b.preco) || (a.preco == b.preco && a.idp < b.idp));

        case descricao:
            /* nega-se o resultado de strcmp para retornar positivo se for inferior */
            return (strcmp(a.descricao, b.descricao) < 0 ? 1 : 0);

        default:
            return -1;
    }
}

int particao(Produto p[], int lo, int hi, criterio c) {
    int i, pivot = p[lo], barreira = lo;

    /* elementos < pivo a esquerda e > pivo a direita*/
    for (i = lo + 1; i <= hi; i++) {
        if (inferior(p[i], pivot, c)) {
            barreira++;
            troca(p, i, barreira);
        }
    }

    troca(p, lo, barreira);
    return barreira;
}

void quicksort(Produto p[], int lo, int hi, criterio c) {
    int pos_pivot;
    if (lo < hi) {
        pos_pivot = particao(p, lo, hi, c);
        quicksort(p, lo, pos_pivot - 1, c);
        quicksort(p, pos_pivot + 1, hi, c);
    }
}


/* Funcao de debug (chamada pelo comando 'D') */
#ifdef DEBUG
    void debug(){
        int i,j;

        /* iterar pelas encomendas */
        for(i = 0; i < armazem.num_encomendas ; i++){

            printf("Encomenda %d\n",i);

            /* iterar pelos produtos da encomenda */
            for(j = 0; j < armazem.encomendas[i].num_produtos_distintos; j++){
                printf("\tProduto:%d Descricao:%s Quantidade:%d\n", armazem.encomendas[i].produtos[j][IDP], 
                                                                    armazem.produtos[armazem.encomendas[i].produtos[j][IDP]].descricao, 
                                                                    armazem.encomendas[i].produtos[j][QTD]);
            }

            printf("\tPeso:%d\n",armazem.encomendas[i].peso);

            putchar('\n');
        }
    }
#endif