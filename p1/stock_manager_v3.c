/*
    * Ficheiro: stock_manager.c
    * Autor: Tomas Philippart, aluno 95683
    * Descricao: Programa
*/

#include <stdio.h>
#include <string.h>

/* Retirar comentario para ativar funcao 
   de debug, executada pelo carater 'D' */
#define DEBUG

#define MAX_DESCRICAO 64
#define MAX_PRODUTOS 10000
#define MAX_ENCOMENDAS 500
#define MAX_PESO 200

/* ------------ MODELIZACAO DO PROBLEMA -------------- 
   Um Armazem contem uma lista de produtos e recebe encomendas.
   As encomendas contem uma lista de items, que referenciam os produtos.
   Decidi usar referencias, em vez de copiar os produtos, para garantir
   que as caracteristicas sao guardadas num so sitio. Assim, por ex:
   se o preco/descricao do produto mudar nao e preciso alterar encomendas.

   Optei por separar o id dos produtos do indice onde esta guardado,
   para permitir (hipoteticamente) apagar produtos sem alterar o seu id.
   Assim, o id e' fixo e independente da estrutura de dados. O unico
   inconveniente e que obriga a procurar o indice, dado o idp, 
   sacrificando desempenho por facilidade de manutencao do codigo. */


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

/* Um item corresponde ao produto e quantidade encomendada. */
typedef struct Item {
    int idp;
    int qtd;
} Item;

/* Uma encomenda e um conjunto de produtos */
typedef struct Encomenda {

   /* Array items da encomenda */
   Item items[MAX_PRODUTOS];

   int peso, num_items_distintos;

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
void novo_prod_encomenda(int ide, int idp, int qtd, int indice);
void remove_stock();
void remove_prod_encomenda();
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

/* ---------- Funcoes auxiliares ---------- */

/* Procura o indice do produto idp no armazem */
int indice_produto_armazem(int idp) {
    int i;
    for (i = 0; i < armazem.num_produtos; i++) {
        if (armazem.produtos[i].idp == idp) {
            return i;
        }
    }
    /* nao encontrou o produto */
    return -1;
}

/* Procura o indice do produto idp na encomenda ide */
int indice_produto_encomenda(int ide, int idp) {
    int i;
    for (i = 0; i < armazem.encomendas[ide].num_items_distintos; i++) {
        if (armazem.encomendas[ide].items[i].idp == idp) {
            return i;
        }
    }
    /* nao encontrou o produto na encomenda */
    return -1;
}

/* ---------- Funcoes de ordenacao ---------- */

void troca(Produto p[], int i, int j) {
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
    int i, barreira = lo;
    Produto pivot = p[lo];

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

/* ---------------------------------------- */


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

    int idp, qtd, indice;
    scanf(" %d:%d", &idp, &qtd);  

    indice = indice_produto_armazem(idp);
    if (indice >= 0) {
        armazem.produtos[indice].stock += qtd;
    } else {
        printf("Impossivel adicionar produto %d ao stock. Produto inexistente.\n", idp);
    }
}


/* Cria uma nova encomenda. */
void nova_encomenda() {

    int ide = armazem.num_encomendas;

    armazem.encomendas[ide].peso = 0;
    armazem.encomendas[ide].num_items_distintos = 0;

    printf("Nova encomenda %d.\n", armazem.num_encomendas);
    armazem.num_encomendas++;
}


/* Adiciona um produto a uma encomenda. Se o produto
   ja existir na encomenda, adiciona quantidade. */
void add_prod_encomenda() {

   int ide, idp, qtd, i, indice;

   scanf(" %d:%d:%d", &ide, &idp, &qtd);

    /* Verificar se a encomenda existe */
    if (ide >= armazem.num_encomendas) {
        printf("Impossivel adicionar produto %d a encomenda %d. Encomenda inexistente.\n", idp, ide);
        return;
    }

    /* Verificar se o produto existe */
    indice = indice_produto_armazem(idp);
    if (indice < 0) {
        printf("Impossivel adicionar produto %d a encomenda %d. Produto inexistente.\n", idp, ide);
        return;
    }

    /* Verificar se a quantidade em stock e suficiente */
    if (qtd > armazem.produtos[indice].stock) {
        printf("Impossivel adicionar produto %d a encomenda %d. Quantidade em stock insuficiente.\n", idp, ide);
        return;
    }

    /* Verificar se o peso da encomenda excede o maximo de 200 */
    if (armazem.encomendas[ide].peso + (armazem.produtos[indice].peso * qtd) > MAX_PESO) {
        printf("Impossivel adicionar produto %d a encomenda %d. Peso da encomenda excede o maximo de 200.\n", idp, ide);
        return;
    }

    /* procurar pelo item na encomenda */
    for (i = 0; i < armazem.encomendas[ide].num_items_distintos; i++) {

        /* se o item ja existir na encomenda, adiciona quantidade */
        if (armazem.encomendas[ide].items[i].idp == idp) {
            armazem.encomendas[ide].items[i].qtd += qtd;

            /* subtrai a quantidade do stock */
            armazem.produtos[indice].stock -= qtd;

            /* aumenta peso*/ 
            armazem.encomendas[ide].peso += armazem.produtos[indice].peso * qtd;
            return;
        } 
    }
    /* se o produto nao estiver na encomenda */
    novo_prod_encomenda(ide, idp, qtd, indice);
}


/* Funcao auxiliar ADICIONA um item novo a uma encomenda */ 
void novo_prod_encomenda(int ide, int idp, int qtd, int indice) {
    /* variavel auxiliar, para nao repetir o comboio */
    int num_item = armazem.encomendas[ide].num_items_distintos;

    /* adiciona o item a encomenda, substituindo o idp e a quantidade */
    armazem.encomendas[ide].items[num_item].idp = idp;
    armazem.encomendas[ide].items[num_item].qtd = qtd;
    
    /* aumenta peso*/ 
    armazem.encomendas[ide].peso += armazem.produtos[indice].peso * qtd;

    /* incrementar o numero de produtos distintos*/
    armazem.encomendas[ide].num_items_distintos++;

    /* subtrai a quantidade do stock */
    armazem.produtos[indice].stock -= qtd;
}


/* Remove stock a um produto existente */
void remove_stock() {
    int idp, qtd_a_remover, indice_prod;
    scanf(" %d:%d", &idp, &qtd_a_remover);

    /* Verificar se o produto existe */
    indice_prod = indice_produto_armazem(idp);
    if (indice_prod < 0) {
        printf("Impossivel remover stock do produto %d. Produto inexistente.\n", idp);
        return;
    }

    /* Verificar se a quantidade em stock e suficiente */
    if (qtd_a_remover > armazem.produtos[indice_prod].stock) {
        printf("Impossivel remover %d unidades do produto %d do stock. Quantidade insuficiente.\n", qtd_a_remover, idp);
    } else {
        armazem.produtos[indice_prod].stock -= qtd_a_remover;
    }
}


/* Remove um produto de uma encomenda */
void remove_prod_encomenda() {
    int ide, idp, qtd, indice_prod, indice_prod_encomenda;

    scanf(" %d:%d", &ide, &idp);

    /* Verificar se a encomenda existe */
    if (ide >= armazem.num_encomendas) {
        printf("Impossivel remover produto %d a encomenda %d. Encomenda inexistente.\n", idp, ide);
        return;
    }

    indice_prod = indice_produto_armazem(idp);
    /* Verificar se o produto existe */
    if (indice_prod < 0) {
        printf("Impossivel remover produto %d a encomenda %d. Produto inexistente.\n", idp, ide);
        return;
    }

    indice_prod_encomenda = indice_produto_encomenda(ide, idp);
    /* se o produto nao estiver na encomenda, nada a remover*/
    if (indice_prod_encomenda < 0 ) {
        return;
    }

    qtd = armazem.encomendas[ide].items[indice_prod_encomenda].qtd;

    /* adiciona stock ao produto a remover */
    armazem.produtos[indice_prod].stock += qtd;

    /* remove peso da encomenda */
    armazem.encomendas[ide].peso -= armazem.produtos[indice_prod].peso * qtd;

    /* colocar quantidade a 0 */
    armazem.encomendas[ide].items[indice_prod_encomenda].qtd = 0;
}


/* Calcula custo da encomenda */
void calcula_custo_encomenda() {
    int i, qtd, ide, idp, indice_prod, custo = 0;

    scanf(" %d", &ide);

    /* Verificar se a encomenda existe */ 
    if (ide >= armazem.num_encomendas) {
        printf("Impossivel calcular custo da encomenda %d. Encomenda inexistente.\n", ide);
        return;
    }

    for (i = 0; i < armazem.encomendas[ide].num_items_distintos; i++) {
        idp = armazem.encomendas[ide].items[i].idp;
        qtd = armazem.encomendas[ide].items[i].qtd;
        indice_prod = indice_produto_armazem(idp);
        custo += (armazem.produtos[indice_prod].preco * qtd);
    }

    printf("Custo da encomenda %d %d.\n", ide, custo);
}


/* Altera o preco de um produto existente no sistema */
void set_preco_prod() {
    int idp, preco, indice_prod;

    scanf(" %d:%d", &idp, &preco);

    /* Verificar se o produto existe */
    indice_prod = indice_produto_armazem(idp);
    if (indice_prod >= 0) {
        armazem.produtos[indice_prod].preco = preco;
    } else {
        printf("Impossivel alterar preco do produto %d. Produto inexistente.\n", idp);
    }
}


/* lista a descrição e a quantidade de um produto numa encomenda */
void lista_descricao_qtd_prod_encomenda() {
    int ide, idp, qtd, indice_prod, indice_prod_encomenda;

    scanf(" %d:%d", &ide, &idp);

    /* Verificar se a encomenda existe */ 
    if (ide >= armazem.num_encomendas) {
        printf("Impossivel listar encomenda %d. Encomenda inexistente.\n", ide);
        return;
    }

    /* Verificar se o produto existe */
    indice_prod = indice_produto_armazem(idp);
    if (indice_prod >= 0) {

        /* Verifica se o produto existe na encomenda */
        indice_prod_encomenda = indice_produto_encomenda(ide, idp);
        if (indice_prod_encomenda >= 0) {
            qtd = armazem.encomendas[ide].items[indice_prod_encomenda].qtd;
            printf("%s %d.\n", armazem.produtos[indice_prod].descricao, qtd);
            
        } else {
            /* se o produto nao existir na encomenda, imprimir com quantidade 0 */
            printf("%s 0.\n", armazem.produtos[indice_prod].descricao);
        }
    } else {
        printf("Impossivel listar produto %d. Produto inexistente.\n", idp);
    }
}

/* lista o identificador da encomenda em que o produto dado ocorre mais vezes */
void lista_ide_max_produto() {

    int i, ide, idp, indice_prod, ide_max_produto = MAX_PRODUTOS, max_quantidade = 0;
    Item item;
    scanf(" %d", &idp);

    /* Verificar se o produto existe */
    indice_prod = indice_produto_armazem(idp);
    if (indice_prod < 0) {
        printf("Impossivel listar maximo do produto %d. Produto inexistente.\n", idp);
        return;
    }
    /* para cada encomenda */
    for (ide = 0; ide < armazem.num_encomendas; ide++) {
        /* para cada item */
        for (i = 0; i < armazem.encomendas[ide].num_items_distintos; i++) {
            item = armazem.encomendas[ide].items[i];
            /* se encontrarmos o item numa dada encomenda*/
            if (item.idp == idp) {
                /* se a quantidade for maior do que a quantidade do maior item,
                esta sera a encomenda onde o item ocorre mais vezes */
                if (item.qtd > max_quantidade || 
                   (item.qtd == max_quantidade && ide < ide_max_produto))  {
                    ide_max_produto = ide;
                    max_quantidade = item.qtd;
                /* se houver 2 ou mais encomendas, seleciona a com menor ide */
                }
            }
        }   
    }
    /* se houver encomendas com esse item: */
    if (max_quantidade > 0) 
        printf("Maximo produto %d %d %d.\n", idp, ide_max_produto, max_quantidade);
}

/* copia os produtos do armazem e devolve o numero de produtos */
int copiar_produtos(Produto p[]) {
    int i, len = armazem.num_produtos;
    for (i = 0; i < len; i++) {
        p[i] = armazem.produtos[i];
    }
    return len;
}


/* lista todos os produtos existentes no 
   sistema por ordem crescente de preco. */
void lista_produtos_por_preco() {
    int i, len;
    criterio c = preco;
    Produto produtos[MAX_PRODUTOS];

    len = copiar_produtos(produtos);

    quicksort(produtos, 0, len - 1, c);

    /* imprimir */
    printf("Produtos\n");
    for (i = 0; i < len; i++) {
        printf("* %s %d %d\n", produtos[i].descricao,
                               produtos[i].preco,
                               produtos[i].stock);
    }
}


/* lista todos os produtos de uma encomenda ide 
   por ordem alfabetica da descricao */
void lista_produtos_alfabeticamente_descricao() {
    int i, ide, idp, indice_prod, len = 0;
    Produto produtos[MAX_PRODUTOS];
    criterio c = descricao;

    scanf(" %d", &ide);

    /* Verificar se a encomenda existe */ 
    if (ide >= armazem.num_encomendas) {
        printf("Impossivel listar encomenda %d. Encomenda inexistente.\n", ide);
        return;
    }

    for (i = 0; i < armazem.encomendas[ide].num_items_distintos; i++) {
        idp = armazem.encomendas[ide].items[i].idp;
        indice_prod = indice_produto_armazem(idp);
        if (indice_prod >= 0) {
            /* copia produto para a tabela temporaria*/
            produtos[len] = armazem.produtos[indice_prod];
            /* altera stock para a quantidade na encomenda ide*/
            produtos[len].stock = armazem.encomendas[ide].items[i].qtd;
            len++;
        }
    }

    quicksort(produtos, 0, len - 1, c);

    /* imprimir */
    printf("Encomenda %d\n", ide);
    for (i = 0; i < len; i++) {
        if (produtos[i].stock > 0) {
            printf("* %s %d %d\n", produtos[i].descricao,
                               produtos[i].preco,
                               produtos[i].stock);
        }
    }
}


/* Funcao de debug (chamada pelo comando 'D') */
#ifdef DEBUG
    void debug(){
        int i, j, indice_prod, idp;

        /* iterar pelas encomendas */
        for(i = 0; i < armazem.num_encomendas ; i++){

            printf("Encomenda %d\n",i);

            /* iterar pelos produtos da encomenda */
            for(j = 0; j < armazem.encomendas[i].num_items_distintos; j++){
                idp = armazem.encomendas[i].items[j].idp;
                indice_prod = indice_produto_armazem(idp);
                printf("\tProduto:%d Descricao:%s Quantidade:%d\n", idp, 
                        armazem.produtos[indice_prod].descricao, 
                        armazem.encomendas[i].items[j].qtd);
            }

            printf("\tPeso:%d\n", armazem.encomendas[i].peso);

            putchar('\n');
        }
    }
#endif