#define _XOPEN_SOURCE 500
#include <unistd.h> 
#include <sys/mman.h>  
#include <fcntl.h>  
#include "utils.h"



int *invalidCount = NULL;  


void create_shared_memory() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    invalidCount = (int *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (invalidCount == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    *invalidCount = 0;

    close(shm_fd);
}

void cleanup_shared_memory() {
    if (invalidCount != NULL) {
        munmap(invalidCount, SHM_SIZE);
    }

    shm_unlink(SHM_NAME);
}

Node* createNode(Booking booking) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    newNode->booking = booking;
    newNode->next = NULL;
    return newNode;
}

void addNode(Node** head, Booking booking) {
    Node* newNode = createNode(booking);

    if (*head == NULL) {
        *head = newNode;
    } else {
        Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void freeList(Node* head) {
    Node* current = head;
    Node* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

void printLinkedList(Node* head) {
    Node* current = head;
    while (current != NULL) {
        printf("Member: %s, Date: %s, Time: %s, Duration: %.1f, Priority: %d, Status: %d, start_slot: %d, end_slot: %d\n", 
               current->booking.member, current->booking.date, current->booking.time,
               current->booking.duration, 
               current->booking.priority, current->booking.status, current->booking.start_slot, current->booking.end_slot);
        int i;
        for (i = 0; i < 6; i++) {
            if (strlen(current->booking.essentials[i]) > 0) {
                printf("  Essential %d: %s\n", i + 1, current->booking.essentials[i]);
            }
        }
        current = current->next;
    }
}

void printoneBooking(Booking booking) {
    printf("Member: %s, Date: %s, Time: %s, Duration: %.2f hours, Essentials: ", 
           booking.member, booking.date, booking.time, booking.duration);
    
    for (int i = 0; i < MAX_ESSENTIALS; i++) {
        if (booking.essentials[i][0] != '\0') { 
            printf("%s", booking.essentials[i]);
            if (i < MAX_ESSENTIALS - 1 && booking.essentials[i + 1][0] != '\0') {
                printf(", "); 
            }
        }
    }
    
    printf(", Priority: %d, Status: %d, Start Slot: %d, End Slot: %d\n", 
           booking.priority, booking.status, booking.start_slot, booking.end_slot);
}

void printarray(Node** nodes, int count) {
    for (int i = 0; i < count; i++) {
        Node* current = nodes[i];
    printf("Member: %s, Date: %s, Time: %s, Duration: %.1f, Priority: %d, Status: %d, start_slot: %d, end_slot: %d, weight: %f\n", 
       current->booking.member, current->booking.date, current->booking.time,
       current->booking.duration, 
       current->booking.priority, current->booking.status, current->booking.start_slot, current->booking.end_slot, 
       current->booking.weight);
        
        for (int j = 0; j < 6; j++) {
            if (strlen(current->booking.essentials[j]) > 0) {
                printf("  Essential %d: %s\n", j + 1, current->booking.essentials[j]);
            }
        }
    }
}
