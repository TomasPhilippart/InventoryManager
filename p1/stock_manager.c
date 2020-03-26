/*
    * Ficheiro: stock_manager.c
    * Autor: Tomas Philippart, aluno 95683
    * Descricao: Programa
*/

#include <stdio.h>

#define MAX_DESCRICAO 64
#define MAX_PRODUTOS 10000
#define MAX_ENCOMENDAS 500
#define MAX_PESO 200

/* estes 2 defines sao usados para os parametros da 
   matriz 2x2 conj_produtos, na estrutura Encomenda */
#define IDP 0
#define QTD 1

typedef struct Produto {
    /*
    Cada produto e' caraterizado por:
       > Identificador (int idp) - [0, 9 999]
       > Descricao (int descricao) - max 63 carateres
       > Preco (int preco) - (> 0)
       > Peso (int peso) - (> 0)
       > Quantidade em stock (int qtd) - (>= 0) */

    int preco, peso, qtd;
    char descricao[MAX_DESCRICAO];

}Produto;



typedef struct Encomenda {
    /* 
    Uma encomenda e' um conjunto de produtos.  
       > Cada encomenda tem as seguintes propriedades:
            » Identificador (int ide) - [0 - 499]
            » Peso (int peso) - (<= 200 unidades)
            » Numero de produtos distintos na encomenda
    */

   /* Array conj_produtos que guarda os produtos na encomenda 
      na forma [[idp_0, preco_0], [idp_1, preco_1], ...] */
   int conj_produtos[MAX_PRODUTOS][2];

   int peso, num_produtos_distintos;

}Encomenda;



/* Tabela com capacidade para
   armazenar todos os produtos */
Produto produtos[MAX_PRODUTOS];

/* Variavel global para manter em conta os produtos e o seu id */
int idp_max = 0;

/* Tabela com capacidade para armazenar todas as encomendas */
Encomenda encomendas[MAX_ENCOMENDAS];

/* Variavel global para manter em conta as encomendas e o seu id */
int ide_max = 0;

/* Declaracao funcoes */
void debug();

void novo_produto();
void add_stock();
void nova_encomenda();
void add_prod_encomenda();
void remove_stock();
void remove_prod_encomenda();
void calcula_custo_encomenda();
void set_preco_prod();
void lista_descricao_qtd_prod_encomenda();
void lista_ide_max_produto();
void lista_produtos_por_preco();
void lista_produtos_alfabeticamente_descricao() {};

int main() {

    int ch;

    while (1) {

        switch(ch = getchar()) {

            case 'D':
                debug();
                break;

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


/* Adicona um novo produto ao sistema. */
void novo_produto() {
    scanf(" %[^:]:%d:%d:%d", produtos[idp_max].descricao, &produtos[idp_max].preco, 
                             &produtos[idp_max].peso, &produtos[idp_max].qtd);

    printf("Novo produto %d.\n", idp_max);
    idp_max++; 
}


/* Adiciona stock a um produto existente no sistema. */
void add_stock() {
             
    int idp, qtd;
    scanf(" %d:%d", &idp, &qtd);  

    if (idp >= idp_max) {
        printf("Impossivel adicionar produto %d ao stock. Produto inexistente.\n", idp);
    }
    produtos[idp].qtd += qtd;
}


/* Cria uma nova encomenda. */
void nova_encomenda() {

    encomendas[ide_max].peso = 0;
    encomendas[ide_max].num_produtos_distintos = 0;

    printf("Nova encomenda %d.\n", ide_max);
    ide_max++;
}


/* Adiciona um produto a uma encomenda. Se o produto
   ja existir na encomenda, adiciona quantidade. */
void add_prod_encomenda() {

   int ide, idp, qtd, i, num_produtos;

   scanf(" %d:%d:%d", &ide, &idp, &qtd);

    /* Verificar se a encomenda existe */
    if (ide >= ide_max) {
        printf("Impossivel adicionar produto %d a encomenda %d. Encomenda inexistente.\n", idp, ide);
        return;
    }

    /* Verificar se o produto existe */
    if (idp >= idp_max) {
        printf("Impossivel adicionar produto %d a encomenda %d. Produto inexistente.\n", idp, ide);
        return;
    }

    /* Verificar se a quantidade em stock e suficiente */
    if (qtd > produtos[idp].qtd) {
        printf("Impossivel adicionar produto %d a encomenda %d. Quantidade em stock insuficiente.\n", idp, ide);
        return;
    }

    /* Verificar se o peso da encomenda excede o maximo de 200 */
    if (encomendas[ide].peso + (produtos[idp].peso * qtd) > MAX_PESO) {
        printf("Impossivel adicionar produto %d a encomenda %d. Peso da encomenda excede o maximo de 200.\n", idp, ide);
        return;
    }

    /* procurar pelo produto na encomenda */
    for (i = 0; i < encomendas[ide].num_produtos_distintos; i++) {

        /* se o produto ja existir na encomenda, adiciona quantidade */
        if (encomendas[ide].conj_produtos[i][IDP] == idp) {
            encomendas[ide].conj_produtos[i][QTD] += qtd;

            /* subtrai a quantidade do stock */
            produtos[idp].qtd -= qtd;

            /* aumenta peso*/ 
            encomendas[ide].peso += produtos[idp].peso * qtd;
            return;
        } 
    }

    /* variavel auxiliar, para nao repetir o comboio */
    num_produtos = encomendas[ide].num_produtos_distintos;

    /* adiciona o produto a encomenda, substituindo a quantidade e o identificador */
    encomendas[ide].conj_produtos[num_produtos][QTD] = qtd;
    encomendas[ide].conj_produtos[num_produtos][IDP] = idp;
    
    /* aumenta peso*/ 
    encomendas[ide].peso += produtos[idp].peso * qtd;

    /* incrementar o numero de produtos distintos*/
    encomendas[ide].num_produtos_distintos++;

    /* subtrai a quantidade do stock */
    produtos[idp].qtd -= qtd;

}


/* Remove stock a um produto existente */
void remove_stock() {
    int idp, qtd_a_remover;
    scanf(" %d:%d", &idp, &qtd_a_remover);

    /* Verificar se o produto existe */
    if (idp >= idp_max) {
        printf("Impossivel remover stock do produto %d. Produto inexistente.\n", idp);
        return;
    }

    /* Verificar se a quantidade em stock e suficiente */
    if (qtd_a_remover > produtos[idp].qtd) {
        printf("Impossivel remover stock do produto %d. Quantidade insuficiente.\n", idp);
        return;
    }

    produtos[idp].qtd -= qtd_a_remover;
}


/* Remove um produto de uma encomenda */
void remove_prod_encomenda() {
    int ide, idp, i, j;


    scanf(" %d:%d", &ide, &idp);

    /* Verificar se a encomenda existe */
    if (ide >= ide_max) {
        printf("Impossivel remover produto %d a encomenda %d. Encomenda inexistente.\n", idp, ide);
        return;
    }

    /* Verificar se o produto existe */
    if (idp >= idp_max) {
        printf("Impossivel remover produto %d a encomenda %d. Produto inexistente.\n", idp, ide);
        return;
    }


    /* adiciona stock ao produto a remover */
    produtos[idp].qtd += encomendas[ide].conj_produtos[idp][QTD];

    /* remove peso da encomenda */
    encomendas[ide].peso -= produtos[idp].peso * 
                            encomendas[ide].conj_produtos[idp][QTD];

    /* percorrer o tabela ate encontrar o produto a remover */
    for (i = 0; i < encomendas[ide].num_produtos_distintos; i++) {
        if (encomendas[ide].conj_produtos[i][IDP] == idp) {
            /* encontrado o produto a remover, vou passar os produtos "depois" dele 
               para a frente */
            for (j = idp; j < encomendas[ide].num_produtos_distintos; j++) {
                /* "empurrar" os produtos para tras, substituindo o produto a remover */
                encomendas[ide].conj_produtos[j][IDP] = encomendas[ide].conj_produtos[j + 1][IDP];
                encomendas[ide].conj_produtos[j][QTD] = encomendas[ide].conj_produtos[j + 1][QTD];
            }

            encomendas[ide].num_produtos_distintos--;
            return;
        }
    }
}


/* Calcula custo da encomenda */
void calcula_custo_encomenda() {
    int i, ide, custo = 0;
    Produto produto_aux;

    scanf(" %d", &ide);

    /* Verificar se a encomenda existe */ 
    if (ide > ide_max) {
        printf("Impossivel calcular custo da encomenda %d. Encomenda inexistente.\n", ide);
        return;
    }

    for (i = 0; i < encomendas[ide].num_produtos_distintos; i++) {
        custo += (produtos[encomendas[ide].conj_produtos[i][IDP]].preco * encomendas[ide].conj_produtos[i][QTD]);
    }

    printf("Custo da encomenda %d %d\n", ide, custo);
}


/* Altera o preco de um produto existente no sistema */
void set_preco_prod() {
    int idp, preco;

    scanf(" %d:%d", &idp, &preco);

    /* Verificar se o produto existe */
    if (idp > idp_max) {
        printf("Impossivel alterar preco do produto %d. Produto inexistente.", idp);
        return;
    }

    produtos[idp].preco = preco;
}


/* lista a descrição e a quantidade de um produto numa encomenda */
void lista_descricao_qtd_prod_encomenda() {
    int ide, idp;

    scanf(" %d:%d", &ide, &idp);

    /* Verificar se a encomenda existe */ 
    if (ide > ide_max) {
        printf("Impossivel listar encomenda %d. Encomenda inexistente.\n", ide);
        return;
    }

    /* Verificar se o produto existe */
    if (idp > idp_max) {
        printf("Impossivel listar produto %d. Produto inexistente.", idp);
        return;
    }

    printf("%s %d", produtos[idp].descricao, encomendas[ide].conj_produtos[idp][QTD]);
}


/* lista o identificaidor da encomenda em 
que o produto dado ocorre mais vezes */
void lista_ide_max_produto() {

    int i, j, idp, ide_max_produto, max_quantidade = 0;

    scanf(" %d", &idp);

    /* Verificar se o produto existe */
    if (idp > idp_max) {
        printf("Impossivel listar maximo do produto %d. Produto inexistente.\n", idp);
        return;
    }
    /* iteracao por todas as encomendas */
    for (j = 0; j < ide_max; j++) {

        /* iteracao por todos os produtos da encomenda j */
        for (i = 0; i < encomendas[j].num_produtos_distintos; i++) {
            /* se encontrarmos o produto numa dada encomenda*/
            if (encomendas[j].conj_produtos[i][IDP] == idp) {
                /* se a quantidade for maior do que a quantidade do maior produto,
                esta sera a encomenda onded o produto ocorre mais vezes */
                if (encomendas[j].conj_produtos[i][QTD] > max_quantidade) {
                    ide_max_produto = j;
                    max_quantidade = encomendas[i].conj_produtos[i][QTD];
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




void debug(){
    int i,j;

    /* iterar pelas encomendas */
    for(i = 0; i < ide_max ; i++){

        printf("Encomenda %d\n",i);

        /* iterar pelos produtos da encomenda */
        for(j = 0; j < encomendas[i].num_produtos_distintos; j++){
            printf("\tProduto:%d Descricao:%s Quantidade:%d\n", encomendas[i].conj_produtos[j][IDP], 
                                                                produtos[encomendas[i].conj_produtos[j][IDP]].descricao, 
                                                                encomendas[i].conj_produtos[j][QTD]);
        }

        printf("\tPeso:%d\n",encomendas[i].peso);

        putchar('\n');
    }
}

