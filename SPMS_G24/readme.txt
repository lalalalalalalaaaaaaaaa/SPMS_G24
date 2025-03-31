Project Title: SmartPark: A Smart Parking Management System for PolyU Using OS-Level Process Coordination
Our group members:
HUANG Lingru 23102488D
CUI Mingyue 23096396D
SHEN Linghui 23103195D
WANG Jialu 24140565X
Welcome to our Smart Parking Management System(SPMS)!
Please follow the following instructions so that you could have a better experience to explore our unique design.
1. A brief introduction to SPMS system: Given the situation that the current parking management system of PolyU has a low resource utilization, our carefully structured SPMS system aims to improve resource utilization via three different crucial shceduling algorithms and the methods of inter process communication.
2. Users should run the SPMS program on apollo, first compile the program with command:
" gcc -std=c99 input_module.c output_module.c scheduler.c utils.c resource.c main.c -o <output_file_name> -lrt -lpthread -lm"
3. Then run the program with the command:
"./<output_file_name>"
4. User input format and instructions:
A.  Users can directly enter the booking information or add a batch file with the command "addBatch -<filename>.dat"
However, if you want to use a batch file, please be careful that it is not allowed to have empty lines within the file content. It is acceptable if there are empty lines at the end of the file. It is welcome to use the batch files provided in the zip file.
Another important notice is that to guarantee the success of inter process communication, there is one second delay after adding the batch. Therefore, please wait around 1 second to see the prompt "please enter booking" and then enter the "printBookings XXXX" command.
B. If you want to see the seperate results by applying a certain scheduling algorithm:
a. FCFS - "printBookings -fcfs"
b. PRIO - "printBookings -prio"
c. OPTI - "printBookings -opti"
C If you want to have a look at the summary report of all the scheduling algorithms:
"printBookings -all"
Both upper case and lower case for the input commands are acceptable, which means "addBATCH" and "ADDbatch" are both right. While the batch file name is case sensitive, users shoud make sure it is correct.
5. Use Ctrl+C to exit to the program
6. Three important assumptions:
a. In order to fully reflect the design of each scheduling algorithm, regarding the handling of priority time slots (8am - 8pm), we only consider this case inside priority scheduling, and in fcfs and opti we consider all time slots as the same level.
b. That a booking falls in the priority time slots means that it is the start time slot of the booking that is within 8am--8pm. 
c. About the criteria of how to measure the improvement of the OPTI scheduling algorithm, we focus on the utilization of essentials as the criterion to compare the performances of OPTI and other two algorithms.
7. About the output file, we write all the booking information from the moment the user enters SPMS system until the user exits the program into one output file. For the analysis report, we use the batch004.dat file for your reference.


