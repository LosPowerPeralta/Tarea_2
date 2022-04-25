#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;

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

//AQUI HACER FUNCION A MAYUSCULA

char* AMayuscula( char* str){
    size_t index = 0;
    size_t largo = strlen( str );
    while ( index < largo)
    {
        str[index] = toupper(str[index]);
        index += 1;
    }

    return str;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;

    char* aux1 = (char*) malloc( 51*sizeof(char) );
    char* aux2 = (char*) malloc( 51*sizeof(char) );

    strcpy(aux1, (char*)key1);
    strcpy(aux2, (char*)key2);

    if( strcmp( AMayuscula(aux1) , AMayuscula(aux2) ) == 0 ){
        free(aux1);
        free(aux2);
        return 1; 
    }
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    if(map->size > (map->capacity*70)/100 ) enlarge(map);
    long index = hash(key, map->capacity );
    while(map->buckets[index]!=NULL && map->buckets[index]->key != NULL){
        if(is_equal(key, map->buckets[index]->key)) return;
        index = (index + 1) % map->capacity;
    }

    if(map->buckets[index]){
        map->buckets[index]->key = key;
        map->buckets[index]->value = value;
    }
    else{
        map->buckets[index] = createPair(key, value);
    }
    map->size += 1;
    
}

void enlarge(HashMap * map) {
    
    /*
    HashMap* mapAux = (HashMap*) malloc(sizeof(HashMap));
    mapAux->buckets = map->buckets;
    mapAux->capacity = map->capacity;

    map->capacity *= 2;
    map->buckets = (Pair**) calloc(map->capacity,sizeof(Pair*));
    Pair* aux = firstMap(mapAux);
    while(aux){
        insertMap(map, aux->key, aux->value);
        aux = nextMap(mapAux);
    }
    */

    Pair** aux = map->buckets;
    map->capacity *= 2;
    map->buckets = (Pair**) calloc(map->capacity, sizeof(Pair*));
    map->size = 0;
    long cont = 0;
    while(cont < map->capacity/2){

        if(aux[cont] && aux[cont]->key){
            insertMap(map, aux[cont]->key, aux[cont]->value);
        }
        cont += 1;
    }

}


HashMap * createMap(long capacity) {

    HashMap* nuevoMapa = (HashMap*) malloc(sizeof(HashMap));
    nuevoMapa->capacity = capacity;
    nuevoMapa->size = 0;
    nuevoMapa->current = -1;
    nuevoMapa->buckets = (Pair**) calloc(capacity,sizeof(Pair*));

    return nuevoMapa;
}

void eraseMap(HashMap * map,  char * key) {    
    if(searchMap(map, key)){
        map->buckets[map->current]->key = NULL;
        map->size -= 1;
    }

}

Pair * searchMap(HashMap * map,  char * key) {   
    long index = hash(key, map->capacity );
    
    while(map->buckets[index]!=NULL && map->buckets[index]->key != NULL){
        if(is_equal(key, map->buckets[index]->key)){
            map->current = index;
            return map->buckets[index];
        }
        index = (index + 1) % map->capacity;
    }

    return NULL;
}

Pair * firstMap(HashMap * map) {
    map->current = -1;
    return nextMap(map);
    /*map->current = 0;

    while( (map->buckets[map->current] == NULL || map->buckets[map->current]->key == NULL) &&  map->current < map->capacity - 1 ){
        map->current += 1;
    }

    if(map->buckets[map->current] && map->buckets[map->current]->key != NULL){
        return map->buckets[map->current];
    }
    return NULL;
    */
}

Pair * nextMap(HashMap * map) {
    map->current += 1;
    
    while(map->current < map->capacity){

        if( map->buckets[map->current] && map->buckets[map->current]->key  ){
        return map->buckets[map->current];
        }  

        map->current += 1;
    }
    return NULL;
}
