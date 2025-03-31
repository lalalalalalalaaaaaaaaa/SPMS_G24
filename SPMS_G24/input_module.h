#ifndef INPUT_MODULE_H
#define INPUT_MODULE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"  
#include <stdbool.h> 

#define MAX_BUFFER_SIZE 1024  // define the size of the buffer


bool parseCommonFields(char* buffer, char* member, char* date, char* time, float* duration, 
                    char essentials[6][20]) ;
bool handleAddParking(Node** head, char* buffer);
bool handleAddReservation(Node** head, char* buffer);
bool handleAddEvent(Node** head, char* buffer);
bool handleBookEssentials(Node** head, char* buffer);
bool addBatch(Node** head, const char* filename);
int convert_datetime_to_slot(const char *date, const char *time);


#endif // INPUT_MODULE_H
