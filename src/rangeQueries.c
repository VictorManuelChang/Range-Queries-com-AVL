#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/libfacom.h"
#include "../include/cJSON.h"

typedef struct
{
    char codigo_ibge[8];
    char nome[50];
    double latitude;
    double longitude;
    int capital;
    int codigo_uf;
    int siafi_id;
    int ddd;
    char fuso_horario[50];
} tmunicipios;

char *int_para_string(int number)
{
    char *str = malloc(sizeof(char) * 9);
    sprintf(str, "%d", number);
    return str;
}

char *get_key(void *reg)
{
    return ((tmunicipios *)reg)->codigo_ibge;
}

void *aloca_municipio(char *codigo_ibge, char *nome, double latitude, double longitude, int capital, int codigo_uf, int siafi_id, int ddd, char *fuso_horario)
{
    tmunicipios *municipio = malloc(sizeof(tmunicipios));

    strcpy(municipio->codigo_ibge, codigo_ibge);
    strcpy(municipio->nome, nome);
    municipio->latitude = latitude;
    municipio->longitude = longitude;
    municipio->capital = capital;
    municipio->codigo_uf = codigo_uf;
    municipio->siafi_id = siafi_id;
    municipio->ddd = ddd;
    strcpy(municipio->fuso_horario, fuso_horario);

    return municipio;
}

typedef int titem;

typedef struct cidade
{
    int codigo_ibge;
    char nome[50];
    double latitude;
    double longitude;
    int codigo_uf;
    int ddd;
    struct cidade *prox;
} tcidade;

typedef struct node
{
    titem item;
    struct node *esq;
    struct node *dir;
    struct node *pai;
    int h;
    tcidade *cidades;
} AVLnode;

typedef struct arvAVL
{
    AVLnode *raiz;
} arvAVL;

void insereNaLista(tcidade **cabeca, tcidade *reg)
{
    if (*cabeca == NULL)
    {
        *cabeca = reg;
    }
    else
    {
        tcidade *aux = *cabeca;
        while (aux->prox != NULL)
        {
            aux = aux->prox;
        }
        aux->prox = reg;
    }
}

tcidade *alocaCidade(int codigo_ibge, const char *nome, double latitude, double longitude, int codigo_uf, int ddd)
{
    tcidade *cidade = (tcidade *)malloc(sizeof(tcidade));
    cidade->codigo_ibge = codigo_ibge;
    strcpy(cidade->nome, nome);
    cidade->latitude = latitude;
    cidade->longitude = longitude;
    cidade->codigo_uf = codigo_uf;
    cidade->ddd = ddd;
    cidade->prox = NULL;
    return cidade;
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int altura(AVLnode *arv)
{
    return arv == NULL ? -1 : arv->h;
}

void rd(AVLnode **pparv)
{
    AVLnode *y = *pparv;
    AVLnode *x = y->esq;
    AVLnode *b = x->dir;

    x->dir = y;
    y->esq = b;

    if (b != NULL)
        b->pai = y;
    x->pai = y->pai;
    y->pai = x;

    y->h = max(altura(y->esq), altura(y->dir)) + 1;
    x->h = max(altura(x->esq), altura(x->dir)) + 1;

    *pparv = x;
}

void re(AVLnode **pparv)
{
    AVLnode *x = *pparv;
    AVLnode *y = x->dir;
    AVLnode *b = y->esq;

    y->esq = x;
    x->dir = b;

    if (b != NULL)
        b->pai = x;
    y->pai = x->pai;
    x->pai = y;

    x->h = max(altura(x->esq), altura(x->dir)) + 1;
    y->h = max(altura(y->esq), altura(y->dir)) + 1;

    *pparv = y;
}

void avl_rebalancear(AVLnode **pparv)
{
    if (*pparv == NULL)
        return;
    int fb = altura((*pparv)->esq) - altura((*pparv)->dir);

    if (fb == -2)
    {
        if (altura((*pparv)->dir->esq) <= altura((*pparv)->dir->dir))
        {
            re(pparv);
        }
        else
        {
            rd(&(*pparv)->dir);
            re(pparv);
        }
    }
    else if (fb == 2)
    {
        if (altura((*pparv)->esq->esq) >= altura((*pparv)->esq->dir))
        {
            rd(pparv);
        }
        else
        {
            re(&(*pparv)->esq);
            rd(pparv);
        }
    }
}

AVLnode *avl_insere(AVLnode **pparv, titem item, tcidade *cidade)
{
    if (*pparv == NULL)
    {
        *pparv = (AVLnode *)malloc(sizeof(AVLnode));
        (*pparv)->item = item;
        (*pparv)->esq = NULL;
        (*pparv)->dir = NULL;
        (*pparv)->pai = NULL;
        (*pparv)->h = 0;
        (*pparv)->cidades = cidade;
    }
    else if (item < (*pparv)->item)
    {
        (*pparv)->esq = avl_insere(&(*pparv)->esq, item, cidade);
        if ((*pparv)->esq != NULL)
            (*pparv)->esq->pai = *pparv;
    }
    else if (item > (*pparv)->item)
    {
        (*pparv)->dir = avl_insere(&(*pparv)->dir, item, cidade);
        if ((*pparv)->dir != NULL)
            (*pparv)->dir->pai = *pparv;
    }
    else
    {
        insereNaLista(&(*pparv)->cidades, cidade);
    }

    (*pparv)->h = max(altura((*pparv)->esq), altura((*pparv)->dir)) + 1;
    avl_rebalancear(pparv);
    return *pparv;
}

AVLnode *insereCidade(arvAVL *parv, titem item, tcidade *cidade)
{
    parv->raiz = avl_insere(&parv->raiz, item, cidade);
    return parv->raiz;
}

AVLnode *valorMin(AVLnode *parv)
{
    AVLnode *atual = parv;
    while (atual->esq != NULL)
    {
        atual = atual->esq;
    }
    return atual;
}

AVLnode *sucessor(AVLnode *parv)
{
    if (parv->dir != NULL)
    {
        return valorMin(parv->dir);
    }
    AVLnode *pai = parv->pai;
    while (pai != NULL && parv == pai->dir)
    {
        parv = pai;
        pai = pai->pai;
    }
    return pai;
}

void rangeQuery(AVLnode *atual, int min, int max, int *resultado, int *tamanho)
{
    if (atual == NULL)
    {
        return;
    }

    if (atual->item > min)
    {
        rangeQuery(atual->esq, min, max, resultado, tamanho);
    }

    if (atual->item >= min && atual->item <= max)
    {
        tcidade *listaCidade = atual->cidades;
        while (listaCidade != NULL)
        {
            resultado[*tamanho] = listaCidade->codigo_ibge;
            (*tamanho)++;
            listaCidade = listaCidade->prox;
        }
    }

    if (atual->item < max)
    {
        rangeQuery(atual->dir, min, max, resultado, tamanho);
    }
}

void liberarCidades(tcidade *cidade)
{
    while (cidade != NULL)
    {
        tcidade *temp = cidade;
        cidade = cidade->prox;
        free(temp);
    }
}

void liberarArvore(AVLnode *arv)
{
    if (arv != NULL)
    {
        liberarArvore(arv->esq);
        liberarArvore(arv->dir);
        liberarCidades(arv->cidades);
        free(arv);
    }
}



int *intersecao(int *vet1, int tamanho1, int *vet2, int tamanho2, int *novoTamanho)
{
    
    int *aux = (int *)malloc(sizeof(int) * (tamanho1 < tamanho2 ? tamanho1 : tamanho2));
    int auxTamanho = 0;

    for (int i = 0; i < tamanho1; i++)
    {
        for (int j = 0; j < tamanho2; j++)
        {
            if (vet1[i] == vet2[j])
            {
                int found = 0;
                for (int k = 0; k < auxTamanho; k++)
                {
                    if (aux[k] == vet1[i])
                    {
                        found = 1;
                        break;
                    }
                }
                if (!found)
                {
                    aux[auxTamanho++] = vet1[i];
                }
                break;
            }
        }
    }

    *novoTamanho = auxTamanho;
    return aux;
}

void printMenu()
{
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║          Consulta de Cidades          ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║ 1. Consulta por DDD                   ║\n");
    printf("║ 2. Consulta por Codigo UF             ║\n");
    printf("║ 3. Consulta por Latitude              ║\n");
    printf("║ 4. Consulta por Longitude             ║\n");
    printf("║ 5. Executar Consultas                 ║\n");
    printf("║ 6. Sair                               ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("Escolha uma opcao: ");
}

int main()
{
    FILE *file = fopen("municipios.json", "r");
    if (!file)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *jsonData = (char *)malloc(fileSize + 1);
    fread(jsonData, 1, fileSize, file);
    jsonData[fileSize] = '\0';
    fclose(file);

    cJSON *json = cJSON_Parse(jsonData);
    if (!json)
    {
        printf("Não foi possível analisar o JSON.\n");
        free(jsonData);
        return 1;
    }

    arvAVL dddArvore = {NULL};
    for (int i = 0; i < cJSON_GetArraySize(json); i++)
    {
        cJSON *objeto = cJSON_GetArrayItem(json, i);

        int codigo_ibge = cJSON_GetObjectItem(objeto, "codigo_ibge")->valueint;
        char *nome = cJSON_GetObjectItem(objeto, "nome")->valuestring;
        double latitude = cJSON_GetObjectItem(objeto, "latitude")->valuedouble;
        double longitude = cJSON_GetObjectItem(objeto, "longitude")->valuedouble;
        int codigo_uf = cJSON_GetObjectItem(objeto, "codigo_uf")->valueint;
        int ddd = cJSON_GetObjectItem(objeto, "ddd")->valueint;

        tcidade *novaCidade = alocaCidade(codigo_ibge, nome, latitude, longitude, codigo_uf, ddd);
        insereCidade(&dddArvore, novaCidade->ddd, novaCidade);
    }

    arvAVL codigoUfArvore = {NULL};
    for (int i = 0; i < cJSON_GetArraySize(json); i++)
    {
        cJSON *objeto = cJSON_GetArrayItem(json, i);

        int codigo_ibge = cJSON_GetObjectItem(objeto, "codigo_ibge")->valueint;
        char *nome = cJSON_GetObjectItem(objeto, "nome")->valuestring;
        double latitude = cJSON_GetObjectItem(objeto, "latitude")->valuedouble;
        double longitude = cJSON_GetObjectItem(objeto, "longitude")->valuedouble;
        int codigo_uf = cJSON_GetObjectItem(objeto, "codigo_uf")->valueint;
        int ddd = cJSON_GetObjectItem(objeto, "ddd")->valueint;

        tcidade *novaCidade = alocaCidade(codigo_ibge, nome, latitude, longitude, codigo_uf, ddd);
        insereCidade(&codigoUfArvore, novaCidade->codigo_uf, novaCidade);
    }

    arvAVL latitudeArvore = {NULL};
    for (int i = 0; i < cJSON_GetArraySize(json); i++)
    {
        cJSON *objeto = cJSON_GetArrayItem(json, i);

        int codigo_ibge = cJSON_GetObjectItem(objeto, "codigo_ibge")->valueint;
        char *nome = cJSON_GetObjectItem(objeto, "nome")->valuestring;
        double latitude = cJSON_GetObjectItem(objeto, "latitude")->valuedouble;
        double longitude = cJSON_GetObjectItem(objeto, "longitude")->valuedouble;
        int codigo_uf = cJSON_GetObjectItem(objeto, "codigo_uf")->valueint;
        int ddd = cJSON_GetObjectItem(objeto, "ddd")->valueint;

        tcidade *novaCidade = alocaCidade(codigo_ibge, nome, latitude, longitude, codigo_uf, ddd);
        insereCidade(&latitudeArvore, novaCidade->latitude, novaCidade);
    }

    arvAVL longitudeArvore = {NULL};
    for (int i = 0; i < cJSON_GetArraySize(json); i++)
    {
        cJSON *objeto = cJSON_GetArrayItem(json, i);

        int codigo_ibge = cJSON_GetObjectItem(objeto, "codigo_ibge")->valueint;
        char *nome = cJSON_GetObjectItem(objeto, "nome")->valuestring;
        double latitude = cJSON_GetObjectItem(objeto, "latitude")->valuedouble;
        double longitude = cJSON_GetObjectItem(objeto, "longitude")->valuedouble;
        int codigo_uf = cJSON_GetObjectItem(objeto, "codigo_uf")->valueint;
        int ddd = cJSON_GetObjectItem(objeto, "ddd")->valueint;

        tcidade *novaCidade = alocaCidade(codigo_ibge, nome, latitude, longitude, codigo_uf, ddd);
        insereCidade(&longitudeArvore, novaCidade->longitude, novaCidade);
    }

    int dddQuery = 0, codigoUfQuery = 0, latitudeQuery = 0, longitudeQuery = 0;
    int dddMin, dddMax, codigoUfMin, codigoUfMax, latitudeMin, latitudeMax, longitudeMin, longitudeMax;

    thash h;
    int nbuckets = 30000;
    hash_constroi(&h, nbuckets, get_key);

    for (int i = 0; i < cJSON_GetArraySize(json); i++)
    {
        cJSON *objeto = cJSON_GetArrayItem(json, i);

        int codigo_ibgeAux = cJSON_GetObjectItem(objeto, "codigo_ibge")->valueint;
        char *nome = cJSON_GetObjectItem(objeto, "nome")->valuestring;
        double latitude = cJSON_GetObjectItem(objeto, "latitude")->valuedouble;
        double longitude = cJSON_GetObjectItem(objeto, "longitude")->valuedouble;
        int capital = cJSON_GetObjectItem(objeto, "capital")->valueint;
        int codigo_uf = cJSON_GetObjectItem(objeto, "codigo_uf")->valueint;
        int siafi_id = cJSON_GetObjectItem(objeto, "siafi_id")->valueint;
        int ddd = cJSON_GetObjectItem(objeto, "ddd")->valueint;
        char *fuso_horario = cJSON_GetObjectItem(objeto, "fuso_horario")->valuestring;

        char *codigo_ibge = int_para_string(codigo_ibgeAux);
        hash_insere(&h, aloca_municipio(codigo_ibge, nome, latitude, longitude, capital, codigo_uf, siafi_id, ddd, fuso_horario));
        free(codigo_ibge);
    }

    while (1)
    {
        printMenu();
        int escolha;
        scanf("%d", &escolha);

        switch (escolha)
        {
        case 1:
            printf("Informe o intervalo de DDD (min max): ");
            scanf("%d %d", &dddMin, &dddMax);
            dddQuery = 1;
            break;
        case 2:
            printf("Informe o intervalo de Codigo UF (min max): ");
            scanf("%d %d", &codigoUfMin, &codigoUfMax);
            codigoUfQuery = 1;
            break;
        case 3:
            printf("Informe o intervalo de Latitude (min max): ");
            scanf("%d %d", &latitudeMin, &latitudeMax);
            latitudeQuery = 1;
            break;
        case 4:
            printf("Informe o intervalo de Longitude (min max): ");
            scanf("%d %d", &longitudeMin, &longitudeMax);
            longitudeQuery = 1;
            break;
        case 5:
        {
            int resultado[5570], resultado2[5570], resultado3[5570], resultado4[5570];
            int tamanho = 0, tamanho2 = 0, tamanho3 = 0, tamanho4 = 0;

            if (dddQuery)
            {
                rangeQuery(dddArvore.raiz, dddMin, dddMax, resultado, &tamanho);
                printf("\n");
            }

            if (codigoUfQuery)
            {
                rangeQuery(codigoUfArvore.raiz, codigoUfMin, codigoUfMax, resultado2, &tamanho2);
                printf("\n");
            }

            if (latitudeQuery)
            {
                rangeQuery(latitudeArvore.raiz, latitudeMin, latitudeMax, resultado3, &tamanho3);
                printf("\n");
            }

            if (longitudeQuery)
            {
                rangeQuery(longitudeArvore.raiz, longitudeMin, longitudeMax, resultado4, &tamanho4);
                printf("\n");
            }

            int *resultadoFinal = NULL;
            int finalSize = 0;

            if (dddQuery && tamanho > 0)
            {
                resultadoFinal = (int *)malloc(sizeof(int) * tamanho);
                memcpy(resultadoFinal, resultado, sizeof(int) * tamanho);
                finalSize = tamanho;
            }
            else if (codigoUfQuery && tamanho2 > 0)
            {
                resultadoFinal = (int *)malloc(sizeof(int) * tamanho2);
                memcpy(resultadoFinal, resultado2, sizeof(int) * tamanho2);
                finalSize = tamanho2;
            }
            else if (latitudeQuery && tamanho3 > 0)
            {
                resultadoFinal = (int *)malloc(sizeof(int) * tamanho3);
                memcpy(resultadoFinal, resultado3, sizeof(int) * tamanho3);
                finalSize = tamanho3;
            }
            else if (longitudeQuery && tamanho4 > 0)
            {
                resultadoFinal = (int *)malloc(sizeof(int) * tamanho4);
                memcpy(resultadoFinal, resultado4, sizeof(int) * tamanho4);
                finalSize = tamanho4;
            }

            if (codigoUfQuery && finalSize > 0 && tamanho2 > 0)
            {
                int *aux = intersecao(resultadoFinal, finalSize, resultado2, tamanho2, &finalSize);
                free(resultadoFinal);
                resultadoFinal = aux;
            }

            if (latitudeQuery && finalSize > 0 && tamanho3 > 0)
            {
                int *aux = intersecao(resultadoFinal, finalSize, resultado3, tamanho3, &finalSize);
                free(resultadoFinal);
                resultadoFinal = aux;
            }

            if (longitudeQuery && finalSize > 0 && tamanho4 > 0)
            {
                int *aux = intersecao(resultadoFinal, finalSize, resultado4, tamanho4, &finalSize);
                free(resultadoFinal);
                resultadoFinal = aux;
            }

            printf("Total de cidades encontradas: %d\n", finalSize);

            printf("Cidades que atendem aos critérios informados:\n");

            if (finalSize > 0)
            {
                printf("Cidades que atendem aos critérios informados:\n");

                for (int i = 0; i < finalSize; i++)
                {
                    char *codigo_buscado = int_para_string(resultadoFinal[i]);

                    if (codigo_buscado == NULL)
                    {
                        fprintf(stderr, "Erro ao converter int para string no resultadoFinal[%d]\n", i);
                        continue;
                    }

                    tmunicipios *municipio = hash_busca(h, codigo_buscado);

                    if (municipio == NULL)
                    {
                        fprintf(stderr, "Cidade com código %s não achado na hash table\n", codigo_buscado);
                        continue;
                    }

                    printf("%d.\n", i + 1); 
                    printf("Nome: %s\n", municipio->nome ? municipio->nome : "N/A");
                    printf("Código IBGE: %s\n", municipio->codigo_ibge ? municipio->codigo_ibge : "N/A");
                    printf("Latitude: %f\n", municipio->latitude);
                    printf("Longitude: %f\n", municipio->longitude);
                    printf("Capital: %d\n", municipio->capital);
                    printf("Codigo_uf: %d\n", municipio->codigo_uf);
                    printf("Siafi_id: %d\n", municipio->siafi_id);
                    printf("DDD: %d\n", municipio->ddd);
                    printf("fuso_horario: %s\n", municipio->fuso_horario ? municipio->fuso_horario : "N/A");

                    printf("\n");
                    free(codigo_buscado);
                }
            }
            else
            {
                printf("Nenhuma cidade atende aos critérios informados.\n");
            }

            printf("Esses são as cidades consultadas, para continuar aperte 'c'\n");

            while (1)
            {
                char entrada;
                scanf(" %c", &entrada); 
                if (entrada == 'c')
                {
                    break;
                }
                else
                {
                    printf("Entrada inválida, tente novamente.\n");
                }
            }

            free(resultadoFinal);
            dddQuery = codigoUfQuery = latitudeQuery = longitudeQuery = 0;
        }
        break;
        case 6:
            hash_apaga(&h);
            liberarArvore(dddArvore.raiz);
            liberarArvore(codigoUfArvore.raiz);
            liberarArvore(latitudeArvore.raiz);
            liberarArvore(longitudeArvore.raiz);
            free(jsonData);
            cJSON_Delete(json);
            return 0;
        default:
            printf("Opcao invalida! Tente novamente.\n");
        }
    }
}
