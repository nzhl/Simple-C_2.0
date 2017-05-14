#include <stdio.h>
#include <string.h>

#include "test.h"
#include "../map.h"

static void test_make_map(){
    printf("\n\nCurrent Function : %s\n\n", __FUNCTION__);
    Map *map = make_map();
    expecti(0, map_size(map));
    expectp(NULL, map_get(map, "key"));
    destroy_map(map);
}

static void test_make_map_parent(){
    printf("\n\nCurrent Function : %s\n\n", __FUNCTION__);
    Map *parent = make_map();
    map_put(parent, "key", "value");

    Map *map = make_map_parent(parent);
    expecti(0, map_size(map));
    expecti(0, strcmp("value", map_get(map, "key")));
    destroy_map(parent);
    destroy_map(map);
}

static void test_map_put(){
    printf("\n\nCurrent Function : %s\n\n", __FUNCTION__);

    Map *map = make_map();
    char *list[] = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14", "15", "16", "17", "18"
    };

    for(int i = 0; i < 18; ++i){
        map_put(map, list[i], list[i]);
    }
    expecti(18, map_size(map));
    for (int i = 0; i < 18; ++i) {
        expecti(0, strcmp(list[i], map_get(map, list[i])));
    }

    destroy_map(map);
}

static void test_map_remove(){
    printf("\n\nCurrent Function : %s\n\n", __FUNCTION__);
    Map *map = make_map();
    map_put(map, "key", "value");
    expecti(1, map_size(map));
    expecti(0, strcmp("value", map_get(map, "key")));
    map_remove(map, "key");
    expectp(NULL, map_get(map, "key"));
    expecti(0, map_size(map));
    destroy_map(map);
}


void test(){
    test_make_map();
    test_make_map_parent();
    test_map_put();
    test_map_remove();
}
