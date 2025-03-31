#include "input_module.h"
#include <strings.h>
#include <math.h> 
#include <stdbool.h> 
#include <errno.h> 
#include "utils.h"

int convert_datetime_to_slot(const char *date, const char *time) {
    // we split the date information into small parts
    int year, month, day;
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3) {
        printf("Error: Invalid date or time format. Request ignored.\n");
        (*invalidCount)++;
        
        return 0;
    }
    if (year != 2025 || month != 5 || day < 10 || day > 16) {
        printf("Error: Invalid date or time format. Request ignored.\n");
        (*invalidCount)++;
        return 0; // the input date is not valid since it is out of range
    }

    int hour;
    if (sscanf(time, "%d", &hour) != 1) {
        printf("Error: Invalid date or time format. Request ignored.\n");
        (*invalidCount)++;
        return 0;
    }

    

    // We assume the base time is 2025-05-10 00:00:00
    int base_day = 10; 
    int day_offset = day - base_day; 
    return day_offset * 24 + hour; // time_slot = day_offset * 24 + hour
}

bool parseCommonFields(char* buffer, char* member, char* date, char* time, float* duration, char essentials[6][20]) {
    char* token = strtok(buffer, " ");
    token = strtok(NULL, " "); 
    if (token == NULL) {
        printf("-> [Error: Missing member name]\n");
        (*invalidCount)++;
        return 0; // the input format is not valid
    }

    size_t skip = strspn(token, "-–"); 

    // analyze the name of each member
    strncpy(member, token + skip, 9); 
    member[9] = '\0'; 

    if (strcasecmp(member, "member_A") != 0 &&
        strcasecmp(member, "member_B") != 0 &&
        strcasecmp(member, "member_C") != 0 &&
        strcasecmp(member, "member_D") != 0 &&
        strcasecmp(member, "member_E") != 0) {
        printf("-> [Error: Invalid member name '%s']\n", member);
        (*invalidCount)++;
        
        return 0;
    }


    // Here we make sure the formats of the input information is correct
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("-> [Error: Missing date]\n");
        (*invalidCount)++;
        return 0;
    }
    strncpy(date, token, 10); 
    date[10] = '\0';


    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("-> [Error: Missing time]\n");
        (*invalidCount)++;
        return 0;
    }
    strncpy(time, token, 5);
    time[5] = '\0';


    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("-> [Error: Missing duration]\n");
        (*invalidCount)++;
        return 0;
    }
    if (sscanf(token, "%f", duration) != 1 || *duration <= 0) {
        printf("-> [Error: Invalid duration format]\n");
        (*invalidCount)++;
        return 0;
    }

        
    token = strtok(NULL, " ");
    int essentialCount = 0;
    
    
    while (token != NULL && essentialCount < 6) {
    
        if (strlen(token) > 0) { 
            strncpy(essentials[essentialCount], token, 19);
            essentials[essentialCount][19] = '\0'; 
            essentialCount++;
        }
        token = strtok(NULL, " ");
    }
    int i;
    for (i = essentialCount; i < 6; i++) {
        essentials[i][0] = '\0'; 
    }


    // check whether the essentials are in pair
    int hasBattery = 0, hasCable = 0;
    int hasLocker = 0, hasUmbrella = 0;
    int hasInflationService = 0, hasValetPark = 0;

    for (i = 0; i < 6; i++) {
        if (strlen(essentials[i]) > 0) {
            if (strcasecmp(essentials[i], "battery") == 0) hasBattery = 1;
            else if (strcasecmp(essentials[i], "cable") == 0) hasCable = 1;
            else if (strcasecmp(essentials[i], "locker") == 0) hasLocker = 1;
            else if (strcasecmp(essentials[i], "umbrella") == 0) hasUmbrella = 1;
            else if (strcasecmp(essentials[i], "inflationService") == 0) hasInflationService = 1;
            else if (strcasecmp(essentials[i], "valetPark") == 0) hasValetPark = 1;
            else {
                printf("-> [Error: Invalid essential]\n");
                (*invalidCount)++;
                return 0;
            }
        }
    }

    // pair batter with cable
    // pair locker with umbrella
      // pair inflationService with valetPark

    for (i = 0; i < 6; i++) {
        if (strlen(essentials[i]) > 0) {

            if (strcasecmp(essentials[i], "battery") == 0 && !hasCable) {
                strcpy(essentials[essentialCount++], "cable");
            }
      
            else if (strcasecmp(essentials[i], "locker") == 0 && !hasUmbrella) {
                strcpy(essentials[essentialCount++], "umbrella");
            }
          
            else if (strcasecmp(essentials[i], "inflationService") == 0 && !hasValetPark) {
                strcpy(essentials[essentialCount++], "valetPark");
            }
        }
    }

    if (hasCable && !hasBattery) {
        strcpy(essentials[essentialCount++], "battery");
    }

    if (hasUmbrella && !hasLocker) {
        strcpy(essentials[essentialCount++], "locker");
    }
   
    if (hasValetPark && !hasInflationService) {
        strcpy(essentials[essentialCount++], "inflationService");
    }

  
    for (i = essentialCount; i < 6; i++) {
        essentials[i][0] = '\0';
    }


    return 1; 
}

bool handleAddParking(Node** head, char* buffer) {
    char member[10], date[11], time[6];
    float duration;
    char essentials[6][20];

    if (!parseCommonFields(buffer, member, date, time, &duration, essentials)) {
        return 0;
    }

    // calculate the time slots (start_slot & end_slot)
    int start_slot = convert_datetime_to_slot(date, time);
    if (start_slot < 0) {
        printf("-> [Error: time underflow]\n");
        (*invalidCount)++;
        return 0;
    }

    int end_slot = start_slot + (int)ceil(duration);
    if (end_slot > 168) {
        printf("-> [Error: time overflow]\n");
        (*invalidCount)++;
        return 0;
    }
    
    int count=0;
    for (int i = 0; i < 6; i++) {
        if (strlen(essentials[i]) > 0) {
            count++;
        }
    }
    float weight = duration*count/2;

    Booking newBooking;

    strcpy(newBooking.member, member);
    strcpy(newBooking.date, date);
    strcpy(newBooking.time, time);
    newBooking.duration = duration;
    newBooking.start_slot = start_slot;
    newBooking.end_slot = end_slot;
    newBooking.weight =  weight;

    for (int i = 0; i < 6; i++) {
        strcpy(newBooking.essentials[i], essentials[i]);
    }

    newBooking.priority = 2; 
    newBooking.status = 0;  // pending

    //add new booking into linked list
    addNode(head, newBooking);
    return 1;
}

//the implementation of other commands are similar to "addParking"
bool handleAddReservation(Node** head, char* buffer) {
    char member[10], date[11], time[6];
    float duration;
    char essentials[6][20];
    int essentialCount = 0;

    if (!parseCommonFields(buffer, member, date, time, &duration, essentials)) {
        return 0;
        printf("this step is right\n");

    }

    int start_slot = convert_datetime_to_slot(date, time);
    if (start_slot < 0) {
        printf("-> [Error: time underflow]\n");
        (*invalidCount)++;
        return 0;
    }

    int end_slot = start_slot + (int)ceil(duration);
    if (end_slot > 168) {
        printf("-> [Error: time overflow]\n");
        (*invalidCount)++;
        return 0;
    }

    int count=0;
    for (int i = 0; i < 6; i++) {
        if (strlen(essentials[i]) > 0) {
            count++;
        }
    }
   float weight = duration*count/2;


    Booking newBooking;

    strcpy(newBooking.member, member);
    strcpy(newBooking.date, date);
    strcpy(newBooking.time, time);
    newBooking.duration = duration;
    newBooking.start_slot = start_slot;
    newBooking.end_slot = end_slot;
    newBooking.weight=weight;


    int i;
    for (i = 0; i < 6; i++) {
        strcpy(newBooking.essentials[i], essentials[i]);
    }


    newBooking.priority = 3; 
    newBooking.status = 0;  


    addNode(head, newBooking);
    return 1;

}

bool handleAddEvent(Node** head, char* buffer) {
    char member[10], date[11], time[6];
    float duration;
    char essentials[6][20];
    int essentialCount = 0;


    if (!parseCommonFields(buffer, member, date, time, &duration, essentials)) {
        return 0;
    }


    int start_slot = convert_datetime_to_slot(date, time);
    if (start_slot < 0) {
        printf("-> [Error: time underflow]\n");
        (*invalidCount)++;
        return 0;
    }

    int end_slot = start_slot + (int)ceil(duration);
    if (end_slot > 168) {
        printf("-> [Error: time overflow]\n");
        (*invalidCount)++;
        return 0;
    }

    int count=0;
    for (int i = 0; i < 6; i++) {
        if (strlen(essentials[i]) > 0) {
            count++;
        }
    }
    float weight = duration*count/2;
    Booking newBooking;

 
    strcpy(newBooking.member, member);
    strcpy(newBooking.date, date);
    strcpy(newBooking.time, time);
    newBooking.duration = duration;
    newBooking.start_slot = start_slot;
    newBooking.end_slot = end_slot;
    newBooking.weight=weight;

  
    int i;
    for (i = 0; i < 6; i++) {
        strcpy(newBooking.essentials[i], essentials[i]);
    }


    newBooking.priority = 4; 
    newBooking.status = 0;  

    addNode(head, newBooking);
    return 1;

}

bool handleBookEssentials(Node** head, char* buffer) {
    char member[10], date[11], time[6];
    float duration;
    char essentials[6][20];
    int essentialCount = 0;

    
    if (!parseCommonFields(buffer, member, date, time, &duration, essentials)) {
        return 0;
    }

    
    int start_slot = convert_datetime_to_slot(date, time);
    if (start_slot < 0) {
        printf("-> [Error: time underflow]\n");
        (*invalidCount)++;
        return 0;
    }

    int end_slot = start_slot + (int)ceil(duration);
    if (end_slot > 168) {
        printf("-> [Error: time overflow]\n");
        (*invalidCount)++;
        return 0;
    }

    int count=0;
    for (int i = 0; i < 6; i++) {
        if (strlen(essentials[i]) > 0) {
            count++;
        }
    }
   float weight = duration*count/2;
    Booking newBooking;


    strcpy(newBooking.member, member);
    strcpy(newBooking.date, date);
    strcpy(newBooking.time, time);
    newBooking.duration = duration;
    newBooking.start_slot = start_slot;
    newBooking.end_slot = end_slot;
    newBooking.weight=weight;


    
    int i;
    for (i = 0; i < 6; i++) {
        strcpy(newBooking.essentials[i], essentials[i]);
    }

    newBooking.priority = 1; 
    newBooking.status = 0;  

    addNode(head, newBooking);
    return 1;

}



// add bookings from batch files
bool addBatch(Node** head, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Invalid batch file. Request ignored.\n");
        (*invalidCount)++;

        return 0;
    }
    int line_count=0;
    char line[MAX_BUFFER_SIZE];
    while (fgets(line, sizeof(line), file) != NULL) {
        char *semicolon = strchr(line, ';');
        if (semicolon != NULL) {
            *semicolon = '\0';  
        }

        line_count++;
       

        if (strncmp(line, "addParking", 10) == 0) {
            if (!handleAddParking(head, line)) {
                printf("-> [Error: invalid command in batch file: '%s', line %d]\n", line, line_count);
                continue; 
            }
        } else if (strncasecmp(line, "addReservation", 14) == 0) {
            if (!handleAddReservation(head, line)) {
               printf("-> [Error: invalid command in batch file: '%s', line %d]\n", line, line_count);
                continue; 
            }
        } else if (strncasecmp(line, "addEvent", 8) == 0) {
            if (!handleAddEvent(head, line)) {
                printf("-> [Error: invalid command in batch file: '%s', line %d]\n", line, line_count);
                continue; 
            }
        } else if (strncasecmp(line, "bookEssentials", 14) == 0) {
            if (!handleBookEssentials(head, line)) {
               printf("-> [Error: invalid command in batch file: '%s', line %d]\n", line, line_count);
                continue; 
            }
        } else {
            printf("-> [Error: Unknown command in batch file: '%s', line %d]\n", line, line_count);
            (*invalidCount)++;
            continue; 
        }
    }

    fclose(file);
    return 1;
}
