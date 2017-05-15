#ifndef MAP_H
#define MAP_H

typedef struct Map{
    // make the map stackable,
    // especial useful for symbol table
    struct Map *parent;

    char **key;
    void **value;
    int size;

    // nused = nelem + ndeleted
    int nelem;
    int nused;
}Map;

#define EMPTY_MAP ((Map){})

Map *make_map();
Map *make_map_parent(Map *parent);
void destroy_map(Map *);
void *map_get(Map *map, char *key);
void map_put(Map *map, char *key, void *value);
void map_remove(Map *map, char *key);
int map_size(Map *map);

#endif
