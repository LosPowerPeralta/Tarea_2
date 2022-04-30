#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
    char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}

void insertMap(HashMap * map, char * key, void * value) {
    Pair *new = createPair(key, value);
    long i = hash(key, map->capacity);
    float porcentaje = map->size / map->capacity;

    if (porcentaje > 0.7) {
        enlarge(map);
    }

    while (map->buckets[i] != NULL && map->buckets[i]->key != NULL)  {
        i = (i + 1) % map->capacity;
    }
    map->buckets[i] = new;
    map->current = i;
    map->size++;
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    Pair **oldBuckets = map->buckets;
    size_t tamano = map->capacity;
    size_t i;

    map->capacity *= 2;
    map->buckets = (Pair **) calloc(map->capacity, sizeof(Pair *));
    map->size = 0;

    for (i = 0; i < tamano; i++) {
        if (oldBuckets[i] != NULL) {
            insertMap(map, oldBuckets[i]->key, oldBuckets[i]->value);
        }
    }
}


HashMap * createMap(long capacity) {
    HashMap *map = (HashMap *) malloc(sizeof(HashMap));

    map->buckets = (Pair **) calloc(capacity, sizeof(Pair *));
    map->capacity = capacity;
    map->current = -1;
    map->size = 0;

    return map;
}

void eraseMap(HashMap * map,  char * key) {    
    Pair *pairEliminado = searchMap(map, key);

    if (pairEliminado != NULL) {
        pairEliminado->key = NULL;
        map->size--;
    }

}

Pair * searchMap(HashMap * map,  char * key) {   
    long i = hash(key, map->capacity);

    while (!is_equal(key, map->buckets[i]->key)) {
        i = (i + 1) % map->capacity;
        if (map->buckets[i] == NULL) return NULL;
    }

    map->current = i;
    return map->buckets[i];
}

Pair * firstMap(HashMap * map) {
    size_t i;

    if (map->size == 0) return NULL;
    for (i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }

    return NULL;
}

Pair * nextMap(HashMap * map) {
    size_t i;

    if (map->size == 0) return NULL;
    for (i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }

    return NULL;
}