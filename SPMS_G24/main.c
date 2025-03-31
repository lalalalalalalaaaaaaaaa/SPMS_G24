#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "input_module.h"
#include "scheduler.h"
#include "output_module.h"
#include <sys/wait.h>
#include <strings.h>

#define MAX_BUFFER_SIZE 1024
//Here we use the idea of linear topology：
// parent -> child(input_module＋scheduler) -> grandchild(output module including anaylze)

// clear the file
void clear_file(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    fclose(file);
}

// write the data of linked list into pipes
void write_linked_list(int fd, Node* head) {
    Node* current = head;
    while (current != NULL) {
        write(fd, &(current->booking), sizeof(Booking));
        current = current->next;
    }

    Booking end_marker = {0};
    write(fd, &end_marker, sizeof(Booking));
}

// read the data from pipes
Node* read_linked_list(int fd) {
    Node* head = NULL;
    Booking booking;
    while (read(fd, &booking, sizeof(Booking)) > 0) {
        if (booking.member[0] == '\0') {
         //end signal
            break;
        }
        addNode(&head, booking);
    }
    return head;
}

int main() {
    create_shared_memory();

    init_resources();       
    reset_all_resources(); 

    
    clear_file("input_commands.txt");
    clear_file("analysis_report.txt");

    // create the pipes
    int input_to_scheduling[2];  
    int scheduling_to_output[2]; 

    if (pipe(input_to_scheduling) == -1 || pipe(scheduling_to_output) == -1) {
        perror("Pipe Failed");
        exit(1);
    }

    // creat the child processes
    pid_t scheduler_pid = fork();
    if (scheduler_pid < 0) {
        perror("Fork Failed");
        exit(1);
    } else if (scheduler_pid == 0) {

        pid_t output_pid = fork();
        if (output_pid < 0) {
            perror("Fork Failed");
            exit(1);
        } else if (output_pid == 0) {
            close(scheduling_to_output[1]); 
            close(input_to_scheduling[1]); 
            close(input_to_scheduling[0]);

            char buffer[MAX_BUFFER_SIZE];
            Node* head = NULL; 

            while (1) {
                read(scheduling_to_output[0], buffer, 15);
                if (strncmp(buffer, "exit",4) == 0) {
                    break;
                } else if (strcmp(buffer, "bookingschedule") == 0) {
                    char algorithm[10];
                    read(scheduling_to_output[0], algorithm, sizeof(algorithm));
                    head = read_linked_list(scheduling_to_output[0]); 
                    
                    printBookings(head, algorithm);
                } else if (strcmp(buffer, "summaryreporttt") == 0) {
                    Node* fcfs_head = read_linked_list(scheduling_to_output[0]);
                    Node* prio_head = read_linked_list(scheduling_to_output[0]);
                    Node* opti_head = read_linked_list(scheduling_to_output[0]);
                    generatePerformanceReport(fcfs_head, prio_head, opti_head);
                } else{
                    continue;
                }
            }
            close(scheduling_to_output[0]);
            exit(0);
        } else {
            close(scheduling_to_output[0]); 
            close(input_to_scheduling[1]);

            char buffer[MAX_BUFFER_SIZE];
            Node* head = NULL; 
            
// Here we design a set of error handling

            while (1) {
                read(input_to_scheduling[0], buffer, MAX_BUFFER_SIZE);
                
                char *semicolon = strchr(buffer, ';');
                if (semicolon != NULL) {
                    *semicolon = '\0';  
                }

                if (strcasecmp(buffer, "endProgram") == 0) {
                    write(scheduling_to_output[1], "exit", 4);
                    break;
                } else if (strncasecmp(buffer, "addParking", 10) == 0) {
                    if (!handleAddParking(&head, buffer)) {
                        continue; 
                    }
                    write(scheduling_to_output[1], "nothingTodo", 11);
                    printf("-> [Pending]\n");
                } else if (strncasecmp(buffer, "addReservation", 14) == 0) {
                    if (!handleAddReservation(&head, buffer)) {
                        continue; 
                    }
                    write(scheduling_to_output[1], "nothingTodo", 11);
                    printf("-> [Pending]\n");
                } else if (strncasecmp(buffer, "addEvent", 8) == 0) {
                    if (!handleAddEvent(&head, buffer)) {
                        continue; 
                    }
                    write(scheduling_to_output[1], "nothingTodo", 11);
                    printf("-> [Pending]\n");
                } else if (strncasecmp(buffer, "bookEssentials", 14) == 0) {
                    if (!handleBookEssentials(&head, buffer)) {
                        continue; 
                    }
                    write(scheduling_to_output[1], "nothingTodo", 11);
                    printf("-> [Pending]\n");
                } else if (strncasecmp(buffer, "addBatch", 8) == 0) {
                    char filename[100];

                    strncpy(buffer, "addBatch", 8);

                    if (sscanf(buffer + 8, " -%[^;]", filename) == 1) {
                        printf("-> [Pending]\n");
                        addBatch(&head, filename);

                    } else {
                            //if the file name has mistakes
                        printf("-> [Error: Invalid batch file name. Expected format: \" -fileName; \"]\n");
                        (*invalidCount)++;
                        continue;
                    }
                    write(scheduling_to_output[1], "nothingTodo", 11);
                }else if (strncasecmp(buffer, "printBookings", 13) == 0) {
                    strncpy(buffer, "printBookings", 13);
                    char algorithm[10];

                    if (sscanf(buffer + 13, " -%[^;]", algorithm) == 1) {
                    } else {
                        printf("-> [Error: Invalid algorithm command. Expected format: \" -algorithmName; \"]\n");
                        (*invalidCount)++;
                        continue;
                    }

                    if (strcasecmp(algorithm, "fcfs") == 0) {
                        Node* fcfs_head = scheduleFCFS(head);
                        printf("-> [Done!]\n");
                        write(scheduling_to_output[1], "bookingschedule", 15);
                        write(scheduling_to_output[1], algorithm, sizeof(algorithm));
                        write_linked_list(scheduling_to_output[1], fcfs_head);
                    } else if (strcasecmp(algorithm, "prio") == 0) {
                        Node* prio_head = schedulePriority(head);
                        printf("-> [Done!]\n");
                        write(scheduling_to_output[1], "bookingschedule", 15);
                        write(scheduling_to_output[1], algorithm, sizeof(algorithm));
                        write_linked_list(scheduling_to_output[1], prio_head);
                    } else if (strcasecmp(algorithm, "opti") == 0) {
                        Node* opti_head = scheduleOptimized(head);
                        printf("-> [Done!]\n");
                        write(scheduling_to_output[1], "bookingschedule", 15);
                        write(scheduling_to_output[1], algorithm, sizeof(algorithm));
                        write_linked_list(scheduling_to_output[1], opti_head);
                    } else if (strcasecmp(algorithm, "All") == 0) {
                        printf("-> [Done!]\n");
                        Node* fcfs_head = scheduleFCFS(head);
                        Node* prio_head = schedulePriority(head);
                        Node* opti_head = scheduleOptimized(head);
        
                        write(scheduling_to_output[1], "summaryreporttt", 15); // The length is 15 so that, like bookingschedule, grandchild can distinguish by directly parsing 15 bytes the first time
                        write_linked_list(scheduling_to_output[1], fcfs_head);
                        write_linked_list(scheduling_to_output[1], prio_head);
                        write_linked_list(scheduling_to_output[1], opti_head);

                    }else{
                        printf("-> [Error: Unknown algorithm command]\n");
                        (*invalidCount)++;
                        continue;
                    }
                }
                else{
                    printf("-> [Error: Unknown command %s]\n", buffer);
                    (*invalidCount)++;
                    
                    continue;
                }
                
                
            }

            close(input_to_scheduling[0]);
            close(scheduling_to_output[1]);
            wait(NULL); 
            exit(0);
        }
    } else {
  
        close(input_to_scheduling[0]); 
        close(scheduling_to_output[0]);
        close(scheduling_to_output[1]); 

        char command[MAX_BUFFER_SIZE];
        while (1) {

            printf("Please enter booking:\n");
            fgets(command, MAX_BUFFER_SIZE, stdin);
            command[strcspn(command, "\n")] = 0; 

            write(input_to_scheduling[1], command, MAX_BUFFER_SIZE);

            
            if (strncasecmp(command, "endProgram", 10) == 0) {
                break;
            }

            char response[MAX_BUFFER_SIZE];
            sleep(1);
        }

        close(input_to_scheduling[1]);
        wait(NULL); 
        exit(0);
    }
    cleanup_shared_memory();
    return 0;
}
