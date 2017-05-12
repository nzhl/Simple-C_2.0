#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "map.h"


/* This file use FNV to implement the hash map,
 * the following are the basic pseudocode :
 *
 * FNV-1:
 *      hash = offset_basis
 *      for each octet_of_data to be hashed
 *          hash = hash * FNV_prime
 *          hash = hash xor octet_of_data
 *      return hash
 * FNV-1a:
 *      hash = offset_basis
 *      for each octet_of_data to be hashed
 *          hash = hash xor octet_of_data
 *          hash = hash * FNV_prime
 *      return hash
 */

#define INIT_SIZE 16
#define FNV_BASIC_32 2166136261
#define FNV_PRIME_32 16777619

// Convert -1 into a pointer,
// which can't really point to something,
// so could be used as a flag to denote deleted.
#define DELETED_FLAG ((void *)-1)


struct Map{
    // make the map stackable,
    // especial useful for symbol table
    struct Map *parent;

    char **key;
    void **value;
    int size;

    // nused = nelem + ndeleted
    int nelem;
    int nused;
};

//FNV-a hash
static uint32_t hash(char *p){
    uint32_t hash = FNV_BASIC_32;
    while(*p){
        hash ^= *p;
        hash *= FNV_PRIME_32;
        ++p;
    }
    return hash;
}

static Map *do_make_map(Map *parent, int size){
    Map *map = malloc(sizeof(Map));
    map->parent = parent;
    map->key = calloc(size, sizeof(char *));
    map->value = calloc(size, sizeof(void *));
    map->size = size;
    map->nelem = 0;
    map->nused = 0;
    return map;
}

static void maybe_rehash(Map *map){

    // if number of used size (nelem + deleted)
    // is beyond the 70% of the total size
    // collide rate increate, so we do rehash.
    if(map->nused < map->size * 0.7){
        return;
    }

    // if the real used size (nelem exclude the deleted elements)
    // is under 35% of the total size, then do not need to resize.
    // just do rehash is enough.
    int new_size = (map->nelem < map->size * 0.35) ? map->size : map->size * 2;
    char **new_key = calloc(new_size, sizeof(char *));
    void **new_value = calloc(new_size, sizeof(void *));

    int mask = new_size - 1;
    for(int i = 0; i < map->size; ++i){
        if(map->key[i] == NULL || map->key[i] == DELETED_FLAG){
            continue;
        }

        int index = hash(map->key[i]) & mask;
        for(;; index = (index + 1) & mask){
            if(new_key[index] != NULL){
                continue;
            }

            new_key[index] = map->key[i];
            new_value[index] = map->value[i];
            break;
        }
    }

    free(map->key);
    free(map->value);

    map->key = new_key;
    map->value = new_value;
    map->size = new_size;
    map->nused = map->nelem;
}


Map *make_map(){
    return do_make_map(NULL, INIT_SIZE);
}

Map *make_map_parent(Map *parent){
    return do_make_map(parent, INIT_SIZE);
}

void destroy_map(Map *map){
    assert(map);
    free(map->key);
    free(map->value);
    free(map);
}


// won't recursively search parent map.
static void *map_get_locally(Map *map, char *key){
    if(!key){
        return NULL;
    }

    int mask = map->size - 1;
    int index = hash(key) & mask;
    for(; map->key[index] != NULL; index = (index + 1) & mask){
        if(map->key[index] != DELETED_FLAG && !strcmp(map->key[index], key)){
            return map->value[index];
        }
    }
    return NULL;
}

void *map_get(Map *map, char *key){
    void *value = map_get_locally(map, key);
    if(value){
        return value;
    }

    if(map->parent){
        return map_get(map->parent, key);
    }
    return NULL;
}

void map_put(Map *map, char *key, void *value){
    // you can not put a pair with NULL key
    assert(key);
    maybe_rehash(map);

    int mask = map->size - 1;
    int index = hash(key) & mask;
    for(;; index = (index + 1) & mask){
        char *temp_key = map->key[index];
        if(temp_key == NULL || temp_key == DELETED_FLAG){
            map->key[index] = key;
            map->value[index] = value;
            ++map->nelem;
            if(temp_key == NULL){
                ++map->nused;
            }
            return;
        }

        // insert a pair with key already existed
        // will be regarded as update pair value
        if(!strcmp(temp_key, key)){
            map->value[index] = value;
            return;
        }
    }
}

void map_remove(Map *map, char *key){
    if(!key){
        return;
    }

    int mask = map->size - 1;
    int index = hash(key) & mask;
    for(; map->key[index] != NULL; index = (index + 1) & mask){
        if(map->key[index] == DELETED_FLAG || strcmp(map->key[index], key)){
            continue;
        }

        map->key[index] = DELETED_FLAG;
        map->value[index] = NULL;
        --map->nelem;
        return;
    }
}

int map_size(Map *map){
    return map->nelem;
}
