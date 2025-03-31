#ifndef UTILS_H
#define UTILS_H

#define MAX_ESSENTIALS 6 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_NAME "/invalid_count_shm"  // the name of the shared memory
#define SHM_SIZE sizeof(int)           // the size of the shared memory

extern int *invalidCount;  // a pointer to the shared memory


typedef enum {
    ESSENTIALS,
    PARKING,
    RESERVATION,
    EVENT
} BookingType;

typedef struct {
    char member[10];          // member names
    char date[11];            // date (YYYY-MM-DD)
    char time[6];             // time (hh:mm)
    float duration;           // durations（hour）
    char essentials[6][20];  
    int priority;             // priority（1: Essentials, 2: Parking, 3: Reservation, 4: Event）
    int status;               // status（0: Pending, 1: Accepted, 2: Rejected）
    BookingType type;        
    int start_slot;         
    int end_slot;
    float weight;           //used for opti algorithm
} Booking;

// Here we use linked list to store the input booking information
typedef struct Node {
    Booking booking;          
    struct Node* next;        
} Node;

Node* createNode(Booking booking);
void addNode(Node** head, Booking booking);
void freeList(Node* head);
void printLinkedList(Node* head);
void printoneBooking(Booking booking);
void create_shared_memory();  
void cleanup_shared_memory(); 

#endif // UTILS_H