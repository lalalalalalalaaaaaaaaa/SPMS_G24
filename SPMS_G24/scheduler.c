#include "scheduler.h"
#include "resource.h"
#include "utils.h"   
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h> 
#include <strings.h>


pthread_mutex_t schedule_mutex = PTHREAD_MUTEX_INITIALIZER;

// The First Come First Serve Algorithm
//In FCFS, we deal with the bookings by the order they are passed in
//If there exists time clash, the booking would be accepted if there is still resources left; 
// otherwise the booking would be rejected

Node* scheduleFCFS(Node* head) {
     reset_all_resources();
    if (!head) return NULL; 

    Node *new_head = NULL; 
    Node *new_tail = NULL; 

    Node *current = head;  

    while (current) {
        Node *new_node = (Node*)malloc(sizeof(Node));
        new_node->booking = current->booking; 
        new_node->next = NULL;

        if (check_resources(&new_node->booking, new_head)) {
            new_node->booking.status = 1; // ACCEPTED
            allocate_resources(&new_node->booking);
        } else {
            new_node->booking.status = 2; // REJECTED
        }

        if (!new_head) {
            new_head = new_node; 
        } else {
            new_tail->next = new_node; 
        }
        new_tail = new_node; 

        current = current->next; 
    }
    
    return new_head; 
}




// EVENT > RESERVATION > PARKING > ESSENTIALS
int get_priority_value(int priority) {
    return priority;
}

// check whether there is a time conflict by comparing the time slots
bool is_time_conflict(const Booking *a, const Booking *b) {
    return (a->start_slot < b->end_slot) && (a->end_slot > b->start_slot);
}

Node* insert_accepted(Node *accepted_head, Node *new_node) {
    new_node->next = accepted_head;
    return new_node;
}


int is_same_date(Booking *b1, Booking *b2) {
    return (strcmp(b1->date, b2->date) == 0);
}

// check whether the booking is in priority time period（08:00-20:00）
//Our assumption: In the priority time period means the start time slot falls in 8:00--20:00
int is_in_priority_time(Booking *booking) {
    int hour, minute;
    sscanf(booking->time, "%d:%d", &hour, &minute);
    return (hour >= 8 && hour < 20); 
}


// The Priority Scheduling
//We rearrange the order of the input_list, with priority descending
// Then it could be treat as the FCFS problem since the the new list is ordered by priority of event and time

Node* schedulePriority(Node *input_list) {
     reset_all_resources();
    Node *sorted_list = NULL;     
    Node *accepted_list = NULL;  
    Node *rejected_list = NULL;   
    
    Node *current = input_list;
    while (current) {
        Node *next = current->next;

        Node *new_node = (Node*)malloc(sizeof(Node));
        new_node->booking = current->booking; 
        new_node->next = NULL;


    if (!sorted_list) {
        new_node->next = sorted_list;
        sorted_list = new_node;
    } else {
        Node *prev = NULL;
        Node *tmp = sorted_list;
        int inserted = 0;
        
        while (tmp) {
            int current_prio = get_priority_value(tmp->booking.priority);
            int new_prio = get_priority_value(new_node->booking.priority);
            
            if (new_prio > current_prio) {
                if (prev == NULL) {
                    new_node->next = sorted_list;
                    sorted_list = new_node;
                } else {
                    new_node->next = tmp;
                    prev->next = new_node;
                }
                inserted = 1;
                break;
            } else if (new_prio == current_prio) {
                int current_time = is_in_priority_time(&tmp->booking);
                int new_time = is_in_priority_time(&new_node->booking);
                
                if (new_time > current_time) {
                    if (prev == NULL) {
                        new_node->next = sorted_list;
                        sorted_list = new_node;
                    } else {
                        new_node->next = tmp;
                        prev->next = new_node;
                    }
                    inserted = 1;
                    break;
                } else if (new_time == current_time) {
                    prev = tmp;
                    tmp = tmp->next;
                } else {
                    // current booking is not in 8am--8pm
                    prev = tmp;
                    tmp = tmp->next;
                }
            } else {  //current booking is of lower priority
                prev = tmp;
                tmp = tmp->next;
            }
        }
        
        if (!inserted) {
            if (prev) {
                prev->next = new_node;
            } else {
                sorted_list = new_node;
            }
        }
    }
    current = next;
}

    return scheduleFCFS(sorted_list); 
}


// The optimization Scheduling
// It dynamically adjusts request weights by penalizing shared resources in conflicting time slots
//It employs adaptive thresholding to initially admit only top 75% performers
// then gives deferred requests a second-chance evaluation round.


int compare_weights(const void *a, const void *b) {
    const Node *node_a = *(const Node **)a;
    const Node *node_b = *(const Node **)b;
    if (node_a->booking.weight > node_b->booking.weight) return -1;
    if (node_a->booking.weight < node_b->booking.weight) return 1;
    return 0;
}

// check whether two bookings have common essentials
int has_common_essential(const Booking *a, const Booking *b) {
    int count=0;
    for (int i = 0; i < 6 && a->essentials[i][0]; i++) {
        for (int j = 0; j < 6 && b->essentials[j][0]; j++) {
            if (strcasecmp(a->essentials[i], b->essentials[j]) == 0) {
                count++;
            }
        }
    }
    return count;
}

// calculate the dynamic threshold
float calculate_threshold(Node **nodes, int count) {
    if (count == 0) return 0.0f;
    
    float sum = 0.0f;
    for (int i = 0; i < count; i++) {
        sum += nodes[i]->booking.weight;
    }
    return (sum / count) * 0.75f; // adjustable
}


Node* create_node_copy(const Node *src) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) return NULL;
    
    memcpy(&new_node->booking, &src->booking, sizeof(Booking));
    new_node->next = NULL;
    return new_node;
}


Node* build_result_list(Node **nodes, int count, Node *accepted_head) {
    Node *result_head = NULL;
    Node *result_tail = NULL;
    
    Node *current = accepted_head;
    while (current) {
        Node *new_node = create_node_copy(current);
        if (!result_head) {
            result_head = result_tail = new_node;
        } else {
            result_tail->next = new_node;
            result_tail = new_node;
        }
        current = current->next;
    }
    
    for (int i = 0; i < count; i++) {
        if (nodes[i]->booking.status != 1) { 
            Node *new_node = create_node_copy(nodes[i]);
            if (!result_head) {
                result_head = result_tail = new_node;
            } else {
                result_tail->next = new_node;
                result_tail = new_node;
            }
        }
    }
    
    return result_head;
}

Node* scheduleOptimized(Node* head) {
     reset_all_resources();
    if (!head) return NULL;

    int count = 0;
    Node *current = head;
    while (current) {
        count++;
        current = current->next;
    }
    
    Node **nodes = (Node**)malloc(sizeof(Node*) * count);
    current = head;
    for (int i = 0; i < count; i++) {
        nodes[i] = (Node*)malloc(sizeof(Node));
        memcpy(&nodes[i]->booking, &current->booking, sizeof(Booking));
        nodes[i]->next = NULL;
        current = current->next;
    }

    // ordered by the weight descending
    qsort(nodes, count, sizeof(Node*), compare_weights);


    float threshold = calculate_threshold(nodes, count);
    
    // the first round allocation
    Node *accepted_head = NULL;
    Node **deferred = (Node**)malloc(sizeof(Node*) * count);
    int deferred_count = 0;
    
    for (int i = 0; i < count; i++) {
        float penalty = 1.0f;
        Node *accepted = accepted_head;
        
        while (accepted) {
            if (is_time_conflict(&nodes[i]->booking, &accepted->booking)) {
                int commanEssentials = has_common_essential(&nodes[i]->booking, &accepted->booking);
                for(int i=0; i<commanEssentials; i++){
                    penalty *= sqrt(0.9f); // set penalty
                       
                }
            }
            accepted = accepted->next;
        }
        
        nodes[i]->booking.weight *= penalty;
        if (nodes[i]->booking.weight < threshold) {
            nodes[i]->booking.status = 0; // pending
            deferred[deferred_count++] = nodes[i];
            continue;
        }
  
        if (check_resources(&nodes[i]->booking, accepted_head)) {
            nodes[i]->booking.status = 1; 
            allocate_resources(&nodes[i]->booking);
            
            Node *new_node = create_node_copy(nodes[i]);
            new_node->next = accepted_head;
            accepted_head = new_node; 
        } else {
            nodes[i]->booking.status = 2; // REJECTED
        }
    }

    
    // dealing with the pending requests
    if (deferred_count > 0) {
        qsort(deferred, deferred_count, sizeof(Node*), compare_weights);  
        
        for (int i = 0; i < deferred_count; i++) {
            if (check_resources(&deferred[i]->booking, accepted_head)) {
                deferred[i]->booking.status = 1; // ACCEPTED
                allocate_resources(&deferred[i]->booking);
                
                Node *new_node = create_node_copy(deferred[i]);
                new_node->next = accepted_head;
                accepted_head = new_node;
            } else {
                deferred[i]->booking.status = 2; // REJECTED
            }
        }
    }
    
    // the final list
    Node *result = build_result_list(nodes, count, accepted_head);
    
    free(deferred);
    for (int i = 0; i < count; i++) free(nodes[i]);
    free(nodes);
    free_node_list(accepted_head);
    
    return result;
}

void free_node_list(Node *list) {
    Node *current = list;
    while (current) {
        Node *next = current->next;
        free(current);
        current = next;
    }
}

