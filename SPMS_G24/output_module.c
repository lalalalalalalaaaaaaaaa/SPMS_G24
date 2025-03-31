#include "output_module.h"
#include "utils.h"
#include "input_module.h"
#include <strings.h>

// print the booking time table

int timeToMinutes(const char* time) {
    int hours, minutes;
    sscanf(time, "%d:%d", &hours, &minutes);
    return hours * 60 + minutes;
}

void minutesToTime(int totalMinutes, char* output) {
    int hours = (totalMinutes / 60) % 24; 
    int minutes = totalMinutes % 60;
    sprintf(output, "%02d:%02d", hours, minutes);
}


void printBookings(Node* head, char* algorithm) {
    // Open the file for appending
    FILE* file = fopen("analysis_report.txt", "a");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    if (head == NULL) {
        printf("No bookings found.\n");
        fprintf(file, "No bookings found.\n");
        fclose(file);
        return;
    }

    // define all the possible user names
    const char* members[] = {"member_A", "member_B", "member_C", "member_D", "member_E"};
    int num_members = 5;

    // print the booking information that has already been accepted
    printf("*** Parking Booking – ACCEPTED / %s ***\n", algorithm);
    printf("\n");
    fprintf(file, "*** Parking Booking – ACCEPTED / %s ***\n", algorithm);
    fprintf(file, "\n");

    int i;
    for (i = 0; i < num_members; i++) {
        Node* current = head;
        int accepted_count = 0;

        while (current != NULL) {
            if (current->booking.status == 1 && strcmp(current->booking.member, members[i]) == 0) {
                accepted_count++;
            }
            current = current->next;
        }

        if (accepted_count > 0) {
            printf("%s has the following bookings:\n", members[i]);
            printf("Date         Start   End         Type         Device\n");
            printf("==========================================================\n");
            fprintf(file, "%s has the following bookings:\n", members[i]);
            fprintf(file, "Date         Start   End         Type         Device\n");
            fprintf(file, "==========================================================\n");

            current = head;
            while (current != NULL) {
                if (current->booking.status == 1 && strcmp(current->booking.member, members[i]) == 0) {


                    int startMinutes = timeToMinutes(current->booking.time); 
                    int durationMinutes = (int)(current->booking.duration * 60); 
                    int endMinutes = startMinutes + durationMinutes; 

                    char endTime[6]; 
                    minutesToTime(endMinutes, endTime); 

                    printf("%s   %s   %s       %-12s ", current->booking.date, current->booking.time, 
                           endTime, 
                           current->booking.priority == 1 ? "*" : 
                           (current->booking.priority == 2 ? "Parking" : 
                           (current->booking.priority == 3 ? "Reservation" : "Event")));
                    fprintf(file, "%s   %s   %s       %-12s ", current->booking.date, current->booking.time, 
                           endTime, 
                           current->booking.priority == 1 ? "*" : 
                           (current->booking.priority == 2 ? "Parking" : 
                           (current->booking.priority == 3 ? "Reservation" : "Event")));

             
                    int is_first_essential = 1; 
                    int hasEssential=0;
                    int j;
                    for (j = 0; j < 6; j++) {
                        if (strlen(current->booking.essentials[j]) > 0) {
                            if (is_first_essential) {
                                printf("%s\n", current->booking.essentials[j]); 
                                fprintf(file, "%s\n", current->booking.essentials[j]);
                                is_first_essential = 0;
                                hasEssential=1;
                            } else {
                                printf("                                              %s\n", current->booking.essentials[j]);
                                fprintf(file, "                                              %s\n", current->booking.essentials[j]);
                            }
                        }else{
                            if(!hasEssential) {
                                printf("*\n");
                                fprintf(file, "*\n");
                            }
                            break;
                        }
                    }
                    printf("\n");
                    fprintf(file, "\n");
                }
                current = current->next;
            }
            printf("\n"); 
            fprintf(file, "\n");
        }
    }
    printf("- End -\n");
    printf("==========================================================\n\n\n");
    fprintf(file, "- End -\n");
    fprintf(file, "==========================================================\n\n\n");

    // print the information of bookings that are REJECTED
    printf("*** Parking Booking – REJECTED / %s ***\n", algorithm);
    printf("\n");
    fprintf(file, "*** Parking Booking – REJECTED / %s ***\n", algorithm);
    fprintf(file, "\n");

    for (i = 0; i < num_members; i++) {
        Node* current = head;
        int rejected_count = 0; 

        while (current != NULL) {
            if (current->booking.status == 2 && strcmp(current->booking.member, members[i]) == 0) {
                rejected_count++;
            }
            current = current->next;
        }

        if (rejected_count > 0) {
            printf("%s (there are %d bookings rejected):\n", members[i], rejected_count);
            printf("Date         Start   End         Type         Essentials\n");
            printf("==========================================================\n");
            fprintf(file, "%s (there are %d bookings rejected):\n", members[i], rejected_count);
            fprintf(file, "Date         Start   End         Type         Essentials\n");
            fprintf(file, "==========================================================\n");

            current = head;
            while (current != NULL) {
                if (current->booking.status == 2 && strcmp(current->booking.member, members[i]) == 0) {

                     // calculate the end time
                    int startMinutes = timeToMinutes(current->booking.time); 
                    int durationMinutes = (int)(current->booking.duration * 60); 
                    int endMinutes = startMinutes + durationMinutes; 

                    char endTime[6]; 
                    minutesToTime(endMinutes, endTime); 

                    printf("%s   %s   %s       %-12s ", current->booking.date, current->booking.time, 
                           endTime, 
                           current->booking.priority == 1 ? "*" : 
                           (current->booking.priority == 2 ? "Parking" : 
                           (current->booking.priority == 3 ? "Reservation" : "Event")));
                    fprintf(file, "%s   %s   %s       %-12s ", current->booking.date, current->booking.time, 
                           endTime, 
                           current->booking.priority == 1 ? "*" : 
                           (current->booking.priority == 2 ? "Parking" : 
                           (current->booking.priority == 3 ? "Reservation" : "Event")));

                
                    int is_first_essential = 1; 
                    int hasEssential=0;
                    int j;
                    for (j = 0; j < 6; j++) {
                        if (strlen(current->booking.essentials[j]) > 0) {
                            if (is_first_essential) {
                                printf("%s\n", current->booking.essentials[j]);
                                fprintf(file, "%s\n", current->booking.essentials[j]);
                                is_first_essential = 0;
                                hasEssential=1;
                            } else {
                                printf("                                              %s\n", current->booking.essentials[j]); 
                                fprintf(file, "                                              %s\n", current->booking.essentials[j]);
                            }
                        }else{
                            if(!hasEssential) {
                                printf("*\n");
                                fprintf(file, "*\n");
                            }
                            break;
                        }
                    }
                    printf("\n");
                    fprintf(file, "\n");
                }
                current = current->next;
            }
            printf("\n"); 
            fprintf(file, "\n");
        }
    }
    printf("- End -\n");
    printf("==========================================================\n\n\n");
    fprintf(file, "- End -\n");
    fprintf(file, "==========================================================\n\n\n");
    
    // Close the file
    fclose(file);
}


const char *essentials_list[] = {
    "locker", "umbrella", "battery", "cable", "valetPark", "inflationservice"
};
const int essentials_count = 6;  

const int total_slots = 3*7*24;  // three for each, seven day a week, twenty-four hours a day

void calculateUtilization(Node* head, const char *essentials_list[], int essentials_count, FILE* file) {
    float total_duration[essentials_count];
    for (int i = 0; i < essentials_count; i++) {
        total_duration[i] = 0.0;
    }

    // traverse the linked list to calculate the total accepted duration for each resource
    Node* current = head;
    while (current != NULL) {
        if (current->booking.status == 1) { 
            for (int i = 0; i < 6; i++) {  
                for (int j = 0; j < essentials_count; j++) {
                    if (strcasecmp(current->booking.essentials[i], essentials_list[j]) == 0) {
                        total_duration[j] += current->booking.duration;
                        break;
                    }
                }
            }
        }
        current = current->next;
    }

    // the resource utilization
    for (int i = 0; i < essentials_count; i++) {
        float utilization = (total_duration[i] / total_slots) * 100;
        printf("    %s – %.1f%%\n", essentials_list[i], utilization);
        if (file != NULL) {
            fprintf(file, "    %s – %.1f%%\n", essentials_list[i], utilization);
        }
    }
}


void generatePerformanceReport(Node* fcfs_head, Node* prio_head, Node* opti_head) {
    // Open the file for appending
    FILE* file = fopen("analysis_report.txt", "a");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    
    int total_fcfs = 0, assigned_fcfs = 0, rejected_fcfs = 0, invalid_fcfs = 0;
    int total_prio = 0, assigned_prio = 0, rejected_prio = 0, invalid_prio = 0;
    int total_opti = 0, assigned_opti = 0, rejected_opti = 0, invalid_opti = 0;
    
    // summary for FCFS
    Node* current = fcfs_head;
    while (current != NULL) {
        total_fcfs++;
        if (current->booking.status == 1) {
            assigned_fcfs++;
        } else if (current->booking.status == 2) {
            rejected_fcfs++;
        }
        if (current->booking.priority < 1 || current->booking.priority > 4) {
            invalid_fcfs++;
        }
        current = current->next;
    }

    // summary for PRIO
    current = prio_head;
    while (current != NULL) {
        total_prio++;
        if (current->booking.status == 1) {
            assigned_prio++;
        } else if (current->booking.status == 2) {
            rejected_prio++;
        }
        if (current->booking.priority < 1 || current->booking.priority > 4) {
            invalid_prio++;
        }
        current = current->next;
    }

    // summary for OPTI
    current = opti_head;
    while (current != NULL) {
        total_opti++;
        if (current->booking.status == 1) {
            assigned_opti++;
        } else if (current->booking.status == 2) {
            rejected_opti++;
        }
        if (current->booking.priority < 1 || current->booking.priority > 4) {
            invalid_opti++;
        }
        current = current->next;
    }

    // print the summary report
    printf("*** Parking Booking Manager – Summary Report ***\n");
    fprintf(file, "*** Parking Booking Manager – Summary Report ***\n");

    printf("Performance:\n");
    fprintf(file, "Performance:\n");
    
    printf(" For FCFS:\n");
    fprintf(file, " For FCFS:\n");
    printf("    Total Number of Bookings Received: %d (%.1f%%)\n", total_fcfs+*invalidCount, (float)total_fcfs / total_fcfs * 100);
    fprintf(file, "    Total Number of Bookings Received: %d (%.1f%%)\n", total_fcfs+*invalidCount, (float)total_fcfs / total_fcfs * 100);
    printf("          Number of Bookings Assigned: %d (%.1f%%)\n", assigned_fcfs, (float)assigned_fcfs / total_fcfs * 100);
    fprintf(file, "          Number of Bookings Assigned: %d (%.1f%%)\n", assigned_fcfs, (float)assigned_fcfs / total_fcfs * 100);
    printf("          Number of Bookings Rejected: %d (%.1f%%)\n", rejected_fcfs, (float)rejected_fcfs / total_fcfs * 100);
    fprintf(file, "          Number of Bookings Rejected: %d (%.1f%%)\n", rejected_fcfs, (float)rejected_fcfs / total_fcfs * 100);
    printf("    Utilization of Time Slot:\n");
    fprintf(file, "    Utilization of Time Slot:\n");
    calculateUtilization(fcfs_head, essentials_list, essentials_count, file);
    printf("    Invalid request(s) made: %d\n", *invalidCount);
    fprintf(file, "    Invalid request(s) made: %d\n", *invalidCount);

    printf(" For PRIO:\n");
    fprintf(file, " For PRIO:\n");
    printf("    Total Number of Bookings Received: %d (%.1f%%)\n", total_prio+*invalidCount, (float)total_prio / total_prio * 100);
    fprintf(file, "    Total Number of Bookings Received: %d (%.1f%%)\n", total_prio+*invalidCount, (float)total_prio / total_prio * 100);
    printf("          Number of Bookings Assigned: %d (%.1f%%)\n", assigned_prio, (float)assigned_prio / total_prio * 100);
    fprintf(file, "          Number of Bookings Assigned: %d (%.1f%%)\n", assigned_prio, (float)assigned_prio / total_prio * 100);
    printf("          Number of Bookings Rejected: %d (%.1f%%)\n", rejected_prio, (float)rejected_prio / total_prio * 100);
    fprintf(file, "          Number of Bookings Rejected: %d (%.1f%%)\n", rejected_prio, (float)rejected_prio / total_prio * 100);
    printf("    Utilization of Time Slot:\n");
    fprintf(file, "    Utilization of Time Slot:\n");
    calculateUtilization(prio_head, essentials_list, essentials_count, file);
    printf("    Invalid request(s) made: %d\n", *invalidCount);
    fprintf(file, "    Invalid request(s) made: %d\n", *invalidCount);

    printf(" For OPTI:\n");
    fprintf(file, " For OPTI:\n");
    printf("    Total Number of Bookings Received: %d (%.1f%%)\n", total_opti+*invalidCount, (float)total_opti / total_opti * 100);
    fprintf(file, "    Total Number of Bookings Received: %d (%.1f%%)\n", total_opti+*invalidCount, (float)total_opti / total_opti * 100);
    printf("          Number of Bookings Assigned: %d (%.1f%%)\n", assigned_opti, (float)assigned_opti / total_opti * 100);
    fprintf(file, "          Number of Bookings Assigned: %d (%.1f%%)\n", assigned_opti, (float)assigned_opti / total_opti * 100);
    printf("          Number of Bookings Rejected: %d (%.1f%%)\n", rejected_opti, (float)rejected_opti / total_opti * 100);
    fprintf(file, "          Number of Bookings Rejected: %d (%.1f%%)\n", rejected_opti, (float)rejected_opti / total_opti * 100);
    printf("    Utilization of Time Slot:\n");
    fprintf(file, "    Utilization of Time Slot:\n");
    calculateUtilization(opti_head, essentials_list, essentials_count, file);
    printf("    Invalid request(s) made: %d\n", *invalidCount);
    fprintf(file, "    Invalid request(s) made: %d\n", *invalidCount);
    
    // Close the file
    fclose(file);
}