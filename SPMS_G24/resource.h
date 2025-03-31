#ifndef RESOURCE_H
#define RESOURCE_H
#define MAX_RES_TYPE 7

#include <stdbool.h>
#include <pthread.h> 
#include "utils.h"

typedef struct Resource{
    char name[20];
    int total;
    int used[7][24];
    pthread_mutex_t lock;
    struct Resource *pair;
} Resource;

extern Resource resources[];

void reset_all_resources(void); 
bool check_resources(Booking *req, Node *accepted_list);
void allocate_resources(Booking *req); 
void release_resources(Booking *req); 
void init_resources();
#endif // RESOURCE_H