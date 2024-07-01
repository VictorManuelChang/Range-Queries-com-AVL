#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define SEED 0x12345678
#define SEED2 0x21d9f3
#include "../include/libfacom.h"

uint32_t hashf(const char *str, uint32_t h)
{
    /* One-byte-at-a-time Murmur hash */
    for (; *str; ++str)
    {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

int hash_insere(thash *h, void *bucket)
{
    uint32_t hash = hashf(h->get_key(bucket), SEED);
    int pos = hash % h->max;
    int passo = (SEED2 - (hash % SEED2)) % h->max;
    int tentativas = 0;

    /* se estiver cheio */
    if (h->size == h->max - 1)
    {
        free(bucket);
        return EXIT_FAILURE;
    }

    /* fazer a inserção */
    while (h->table[pos] != 0 && h->table[pos] != h->deleted)
    {
        pos = (pos + passo) % h->max;
        tentativas++;
        if (tentativas > h->max)
        { 
            free(bucket);
            return EXIT_FAILURE;
        }
    }

    h->table[pos] = (uintptr_t)bucket;
    h->size++;

    return EXIT_SUCCESS;
}

int hash_constroi(thash *h, int nbuckets, char *(*get_key)(void *))
{
    h->table = calloc(nbuckets + 1, sizeof(void *));
    if (h->table == NULL)
    {
        return EXIT_FAILURE;
    }
    h->max = nbuckets + 1;
    h->size = 0;
    h->deleted = (uintptr_t) & (h->size); 
    h->get_key = get_key;
    return EXIT_SUCCESS;
}

void *hash_busca(thash h, const char *key)
{
    uint32_t hash = hashf(key, SEED);
    int pos = hash % h.max;
    int passo = (SEED2 - (hash % SEED2)) % h.max;
    int tentativas = 0;

    while (h.table[pos] != 0)
    {
        if (strcmp(h.get_key((void *)h.table[pos]), key) == 0)
        {
            return (void *)h.table[pos];
        }
        pos = (pos + passo) % h.max;
        tentativas++;
        if (tentativas > h.max)
        { 
            return NULL;
        }
    }
    return NULL;
}

int hash_remove(thash *h, const char *key)
{
    int pos = hashf(key, SEED) % (h->max);
    while (h->table[pos] != 0)
    {
        if (strcmp(h->get_key((void *)h->table[pos]), key) == 0)
        { /* se achei remove*/
            free((void *)h->table[pos]);
            h->table[pos] = h->deleted;
            h->size -= 1;
            return EXIT_SUCCESS;
        }
        else
        {
            pos = (pos + 1) % h->max;
        }
    }
    return EXIT_FAILURE;
}

void hash_apaga(thash *h)
{
    int pos;
    for (pos = 0; pos < h->max; pos++)
    {
        if (h->table[pos] != 0)
        {
            if (h->table[pos] != h->deleted)
            {
                free((void *)h->table[pos]);
            }
        }
    }
    free(h->table);
}