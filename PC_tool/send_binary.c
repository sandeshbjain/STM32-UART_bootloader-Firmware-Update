#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RS232_library/rs232.h"

#define PACKET_SIZE 1024
#define BAUD_RATE 115200

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

void delay_us(uint32_t us)
{
	us *= 10;
#ifdef _WIN32
	// Sleep(ms);
	__int64 time1 = 0, time2 = 0, freq = 0;

	QueryPerformanceCounter((LARGE_INTEGER *)&time1);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

	do
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&time2);
	} while ((time2 - time1) < us);
#else
	usleep(us);
#endif
}

void sendFile(char* filename, int comport){
	FILE* file = fopen(filename, "rb");
	if(file == NULL){
		printf("File doesnt exist, check again");
		return;
	}
	
	char packet_buffer[PACKET_SIZE];
	char start_buffer[] ="START";
	char stop_buffer[] ="STOP";
	char ack_buffer[3];
	int bytesRead;
	
	if(RS232_OpenComport(comport, BAUD_RATE, "8N1",0)==0){ //8N1 indicates 8 data bits, 0 parity bits and 1 stop bit
		printf("COMPORT CONNECTED at %d \n", comport+1);
		
		delay_us(400000);
		
		if (RS232_SendBuf(comport, start_buffer, sizeof(start_buffer)) < 0) {
                printf("*********Error in sending start bit. Exiting*********\n");
                return;
		}
		else{
			printf("Start Message Sent \n");
			delay_us(400000);
		}	
		
		while ((bytesRead = fread(packet_buffer, 1, PACKET_SIZE, file)) > 0) {
            // Send the packet to the STM32 microcontroller
            if (RS232_SendBuf(comport, packet_buffer, bytesRead) < 0) {
                printf("Error sending data. Exiting...\n");
                return;
			}
			else{
			printf("Sending Data packets \n");
			delay_us(400000);
			
			// After sending each packet, wait for an acknowledgment
			int ackBytes = RS232_PollComport(comport, (unsigned char*)ack_buffer, sizeof(ack_buffer));
			if (ackBytes == 3 && strncmp((char*)ack_buffer, "OK", 2) == 0) {
				printf("Acknowledgment received: OK\n");
			} else {
				printf("Error receiving acknowledgment. Exiting...\n");
				return;
			}
			
			}
			
		}
		
		if (RS232_SendBuf(comport, stop_buffer, sizeof(stop_buffer)) < 0) {
                printf("*********Error in sending sto[ bit. Exiting*********\n");
                return;
		}
		else{
			printf("Stop Message Sent \n");
			delay_us(400000);
		}
		RS232_CloseComport(comport);
        printf("COM%d port closed.\n", comport+1);

		fclose(file);
				
	
	}
	
	
	
}


int main(int argc, char * argv[]){
	
	//expecting 3 arguments ./sendfile, binary_file_name, comport number  
	if(argc != 3){
		printf("Expecting 2 arguements: binary file name and comport number");
		return 1;
	}
	
	char* filename = argv[1];
	int comport = atoi(argv[2])-1;
	
	sendFile(filename, comport);
	
	return 0;
}


// Modification required:
//Send the number of packets along with start command to receive the stop command. Dimag laga bsdkboss