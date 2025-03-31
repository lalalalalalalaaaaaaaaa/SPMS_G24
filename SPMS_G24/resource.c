#include "resource.h"
#include "utils.h"
#include <string.h>
#include <pthread.h>
#include <strings.h>
#include "scheduler.h"


Resource resources[] = {
    {"battery", 3, {{0}}, PTHREAD_MUTEX_INITIALIZER},
    {"cable", 3, {{0}}, PTHREAD_MUTEX_INITIALIZER},
    {"locker", 3, {{0}}, PTHREAD_MUTEX_INITIALIZER},
    {"umbrella", 3, {{0}}, PTHREAD_MUTEX_INITIALIZER},
    {"inflation", 3, {{0}}, PTHREAD_MUTEX_INITIALIZER},
    {"valetPark", 3, {{0}}, PTHREAD_MUTEX_INITIALIZER},
    {"parking", 10, {{0}}, PTHREAD_MUTEX_INITIALIZER, NULL}
};

void reset_all_resources() {
    for (int i = 0; i < 7; i++) {
        pthread_mutex_lock(&resources[i].lock);
        memset(resources[i].used, 0, sizeof(resources[i].used)); 
        pthread_mutex_unlock(&resources[i].lock);
    }
}

void print_remaining_resources(Resource* res) {
    printf("Remaining Resources Summary:\n");
    for (int i = 0; i < 7; i++) {
        printf("Resource: %s\n", res[i].name);
        for (int day = 0; day < 7; day++) { 
            int remaining = res[i].total;
            for (int hour = 0; hour < 24; hour++) {
                remaining -= res[i].used[day][hour];
            }
            printf("  Day %d: %d remaining\n", day, remaining);
        }
        printf("\n");
    }
}

//make a clone
void clone_resources(Resource *dest, const Resource *src, int num) {
    for (int i = 0; i < num; i++) {
        dest[i].total = src[i].total; 
        memset(dest[i].used, 0, sizeof(dest[i].used)); 
        strncpy(dest[i].name, src[i].name, sizeof(dest[i].name) - 1); 
        dest[i].name[sizeof(dest[i].name) - 1] = '\0'; 
    }
}

bool check_resources(Booking *req, Node *accepted_list) {
    Resource temp_resources[MAX_RES_TYPE];
    clone_resources(temp_resources, resources, MAX_RES_TYPE);
    for(int i=0; i<7; i++){
    }

    Node *accepted = accepted_list;
    
    while (accepted) {
        
        if (is_time_conflict(req, &accepted->booking)) {
            if (accepted->booking.priority != 1) {  
                for (int slot = accepted->booking.start_slot; slot < accepted->booking.end_slot; slot++) {
                    int day = slot / 24;
                    int hour = slot % 24;
                    temp_resources[6].used[day][hour]++; 
                }
            }

            for (int i = 0; i < MAX_ESSENTIALS; i++) {
                if (!accepted->booking.essentials[i][0]) break;
                for (int r = 0; r < 6; r++) {
                    if (strcasecmp(accepted->booking.essentials[i], temp_resources[r].name) == 0) {
                        for (int slot = accepted->booking.start_slot; slot < accepted->booking.end_slot; slot++) {
                            int day = slot / 24;
                            int hour = slot % 24;
                            temp_resources[r].used[day][hour]++;
                            
                        }
                    }
                }
            }

        }
        accepted = accepted->next;

    }

    //check whether the resources are still available based on the exsiting requests
    if (req->type != ESSENTIALS) {
        Resource *parking = &temp_resources[6];
        for (int slot = req->start_slot; slot < req->end_slot; slot++) {
            int day = slot / 24;
            int hour = slot % 24;
            if (parking->used[day][hour] >= parking->total) {
                return false;
            }
        }
    }

    for (int i = 0; i < MAX_ESSENTIALS && req->essentials[i][0]; i++) {
        for (int r = 0; r < 6; r++) {
            if (strcmp(req->essentials[i], temp_resources[r].name) == 0) {
                Resource *res = &temp_resources[r];
                for (int slot = req->start_slot; slot < req->end_slot; slot++) {
                    int day = slot / 24;
                    int hour = slot % 24;
                    if (res->used[day][hour] >= res->total) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

// the function for allocating the resources for each booking
void allocate_resources(Booking *req) {
    if (req->type != ESSENTIALS) {
        Resource *parking = &resources[6];
        pthread_mutex_lock(&parking->lock);
        for (int slot = req->start_slot; slot < req->end_slot; slot++) {
            int day = slot / 24;
            int hour = slot % 24;
            parking->used[day][hour]++;
        }
        pthread_mutex_unlock(&parking->lock);
    }

    for (int i = 0; i < MAX_ESSENTIALS && req->essentials[i][0]; i++) {
        for (int r = 0; r < 6; r++) {
            if (strcmp(req->essentials[i], resources[r].name) == 0) {
                Resource *res = &resources[r];
                pthread_mutex_lock(&res->lock);
                for (int slot = req->start_slot; slot < req->end_slot; slot++) {
                    int day = slot / 24;
                    int hour = slot % 24;
                    res->used[day][hour]++;
                }
                pthread_mutex_unlock(&res->lock);
            }
        }
    }
}

void release_resources(Booking *req) {

    if (req->type != ESSENTIALS) {
        Resource *parking = &resources[6];
        pthread_mutex_lock(&parking->lock);
        for (int slot= req->start_slot; slot < req->end_slot; slot++) {
            int day = slot / 24;
            int hour = slot % 24;
            parking->used[day][hour]--;
        }
        pthread_mutex_unlock(&parking->lock);
    }


    for (int i = 0; i < 6 && req->essentials[i][0]; i++) {
        for (int r = 0; r < 6; r++) {
            if (strcmp(req->essentials[i], resources[r].name) == 0) {
                Resource *res = &resources[r];
                pthread_mutex_lock(&res->lock);
                for (int slot = req->start_slot; slot < req->end_slot; slot++) {
                    int day = slot / 24;
                    int hour = slot % 24;
                    res->used[day][hour]--;
                    if (res->pair) {
                        res->pair->used[day][hour]--;
                    }
                }
                pthread_mutex_unlock(&res->lock);
            }
        }
    }
}

void init_resources() {
    for (int i = 0; i < 7; i++) {
        pthread_mutex_init(&resources[i].lock, NULL);
        memset(resources[i].used, 0, sizeof(resources[i].used));
    }
}

void cleanup_resources() {
    for (int i = 0; i < 7; i++) {
        pthread_mutex_destroy(&resources[i].lock); 
    }
}