#ifndef OUTPUT_MODULE_H
#define OUTPUT_MODULE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"  
#include <stdarg.h>


void printBookings(Node* head, char* algorithm);
void generatePerformanceReport(Node* fcfs_head, Node* prio_head, Node* opit_head);
void calculateUtilization(Node* head, const char *essentials_list[], int essentials_count, FILE* file);

#endif //OUTPUT_MODULE_H