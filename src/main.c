// Copyright Thomas Herpoel 2016
//
// This file is part of BaseI6_updater
//
// BaseI6_updater is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// FSi6_updater is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifdef WIN
#include <windows.h>
#else
#include "types.h"
#endif  // WIN

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef WIN
#include <conio.h>
#endif  /* WIN */
#include <sys/stat.h>
#include "app.h"

#ifdef WIN
#define DEFAULT_COMPORT "COM0"
#else
#define DEFAULT_COMPORT "/dev/tty.usbserial"
#endif
#define DEFAULT_BAUDRATE 115200

#define _DEBUG

// prototypes
uint8_t empty(void) ;
uint8_t openPort(void);
uint8_t closePort(void);
uint8_t printHelp(void);
uint8_t detect(void);
uint8_t check(void);
uint8_t upload(void);
uint8_t erase(void);
uint8_t reset(void);
uint8_t readPage(uint8_t page[], uint16_t nbytes,FILE* file,uint16_t offset);

// global variables

char comPortId[512] = DEFAULT_COMPORT;
uint32_t baudrate = DEFAULT_BAUDRATE;
char filePath[512];
bool usePort = false;				// set to true if com port or baudrate is defined
bool autoDetect = true;
bool withBtldr = false;			// set to true by -bt option
bool useCrcPatch = false;		// set to true by -c option
bool verbose = false;				// set to true by -v option

int main(int argc, char *argv[]) {
	uint8_t (*function)(void);	// pointer to the executed command
	function = printHelp;		// by default: print help and quit 

	// parsing command line options
	for(int i = 1; i<argc; i++) {
		printf("Arg: %s\r\n", argv[i]);

		if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--h") == 0 || strcmp(argv[i], "help") == 0) {
			function = printHelp;
		}
		else if(strcmp(argv[i], "-d") == 0) {
			strcpy(comPortId, argv[i+1]);
			usePort = true;
			autoDetect = false;
			i++;
		}
		else if(strcmp(argv[i], "-b") == 0) {
			baudrate = (uint32_t)atoi(argv[i+1]);
			usePort = true;
			i++;
		}
		else if(strcmp(argv[i], "-bt") == 0) {
			withBtldr = true;
		}
		else if(strcmp(argv[i], "-c") == 0) {
			useCrcPatch = true;
		}
		else if(strcmp(argv[i], "-p") == 0) {
			strcpy(filePath, argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i], "-v") == 0) {
			verbose = true;
		}
		else if(strcmp(argv[i], "read") == 0) {
			function = detect;
			usePort = true;
		}
		else if(strcmp(argv[i], "flash") == 0) {
			function = upload;
			usePort = true;
		}
		else if(strcmp(argv[i], "erase") == 0) {
			function = erase;
			usePort = true;
		}
		else if(strcmp(argv[i], "reset") == 0) {
			function = reset;
			usePort = true;
		}
		else {
			printf("Invalid option: %s. \r\n", argv[i]);
			function = printHelp;
			break;
		}
	}
	
	if(usePort) {
		if(openPort() != 0) {
			printf("Failed to open port %s.\r\n", comPortId);
			if(verbose) {
				printf("Press any key to exit...\r\n");
				getchar();
			}
			return -1;
		}
	}
			
	// execute selected function
	(*function)();
	
	// close serial port
	if(usePort) closePort();
	
	// bause execution (verbose mode)
	if(verbose) {
		printf("Press any key to exit...\r\n");
		getchar();
	}
	
	return 0;
}

uint8_t empty(void) {
	return 0;
}

uint8_t openPort(void) {
	printf("Connecting to %s at %d bps...\t", comPortId, baudrate);

	if(appInit(comPortId, baudrate) != 0) {
		printf("FAIL\r\n");
		return -1;
	}

	printf("OK\r\n");

	if(detect() != 0) {
		closePort();
		return -1;
	}

	return 0;
}

uint8_t closePort(void) {
	printf("Closing port %s...\t", comPortId);

	if(appDeinit() != 0) {
		printf("FAIL\r\n");
		return -1;
	}

	printf("OK\r\n");
	return 0;
}

uint8_t printHelp(void){
	printf("\r\n");
	printf("Usage: BaseI6_updater [options] action \r\n\r\n");
	printf("Actions:\r\n");
	printf(" read          : detect firmware version\r\n");
	printf(" flash         : upload firware image\r\n");
	printf(" erase         : erase transmitter firmware\r\n");
	printf(" reset         : reset transmitter\r\n");
	printf("\r\nOptions:\r\n");
	printf(" -b rate       : com port baudrate\r\n");
	printf(" -bt           : if image file contains bootloader\r\n");
	printf(" -c            : check flash image CRC before upload\r\n");
	printf(" -d (path|com) : serial device path or com port (eg. '/dev/cu.usbserial' or 'COM1')\r\n");
	printf(" -h            : print help and quit\r\n");
	printf(" -p path       : path to flash image\r\n");	
	printf(" -v            : verbose mode (pause at the end)\r\n");
	printf("\r\n");
	return 0;
}

uint8_t detect(void) {
	printf("Reading i6 firmware...\t");

	if(appDetect() != 0) {
		printf("FAIL\r\n");
		return -1;
	}
	printf("*OK\r\n");
	return 0;
}

uint8_t check(void) {
	printf("*Checking flash image CRC:\r\n");
	char cmd[256];
	int code = -1;
	
	if(withBtldr) sprintf(cmd, "tools\\BaseI6_CRC_patcher %s -b", filePath);
	else sprintf(cmd, "tools\\BaseI6_CRC_patcher %s", filePath);
	code = system(cmd);

	if(code==0)
		printf("*\t\t\tSUCCESS\r\n");
	else
		printf("*\t\t\tERROR\r\n");

	return code;
}

uint8_t upload(void) {
	// if(detect()!=0) {
		// return -1;
	// }
	if(useCrcPatch) {
		check();
	}
	printf("*Uploading:\t\t%s\r\n",filePath);
	printf("*\tOpening file\t");
	
	FILE* file = NULL;
	file = fopen(filePath, "rb");
	if(file==NULL)
		printf("[FAIL]\r\n");
	else
		printf("done\r\n");
	
	struct stat st;
	stat(filePath, &st);
	uint16_t fileSize = st.st_size;
	printf("*\tFile size\t0x%X bytes\r\n",fileSize);
	
	
	uint16_t start = 0x1800;
	uint16_t finish;
	if(withBtldr)
		finish = fileSize;
	else
		finish = 0x1800+fileSize;
	printf("*\t");
	for(int offset=start;offset<finish;offset+=0x80) {
		if(offset%0x400 == 0) {
			printf("_");
		}
	}
	printf("\r\n*\t");
	
	uint16_t bytesRemaining = fileSize;
	if(withBtldr)
		bytesRemaining -= 0x1800;
	uint8_t data[512];
	uint16_t lastOffset=0x1800;
	for(int offset=start;offset<finish-0x80;offset+=0x80) {
		if(withBtldr)
			readPage(data,0x80,file,offset);
		else
			readPage(data,0x80,file,offset-0x1800);
		
		if(offset%0x400 == 0) {
			if(appPrepareBlock(offset)!=0) {
				return -1;
			}
			else
				printf("%c",178);
		}
		appWritePage(data,0x80, offset);
		bytesRemaining -= 0x80;
		lastOffset=(uint16_t)offset+0x80;
	}
	printf("\r\n");
	//printf("writing 0x%X bytes at 0x%X\r\n",bytesRemaining,lastOffset);
	if(bytesRemaining>0) {
		if(withBtldr)
			readPage(data,bytesRemaining,file,lastOffset);
		else
			readPage(data,bytesRemaining,file,lastOffset-0x1800);
		appWritePage(data,bytesRemaining,lastOffset);
	}
	

	printf("*\t\t\tSUCCESS\r\n");
	fclose(file);

	
	reset();
	
	return 0;
}

uint8_t readPage(uint8_t page[], uint16_t nbytes,FILE* file,uint16_t offset)
{
	fseek(file, offset, 0);
  for(int i=0;i<nbytes;i++) {
      // printf("index : %x\n",(int)ftell(file));
      page[i]=(uint8_t)fgetc(file);
  }
  return 0;
}


uint8_t erase(void) {
	uint16_t start = 0x1800;
	uint16_t finish = 0xFFFF;
	
	if(detect()!=0) {
		return -1;
	}
	printf("*Erasing:\r\n*\t");
	uint8_t blankPage[0x80];
	for(int i =0;i<sizeof(blankPage);i++) {
		blankPage[i]=0xFF;
	}
	for(int offset=start;offset<finish;offset+=0x80) {
		if(offset%0x400 == 0) {
			printf("_");
		}
	}
	printf("\r\n*\t");
	for(int offset=start;offset<finish;offset+=0x80) {
		if(offset%0x400 == 0) {
			if(appPrepareBlock(offset)!=0) {
				return -1;
			}
			else
				printf("%c",178);
		}
		appWritePage(blankPage,sizeof(blankPage), offset);
	}
	
	printf("\r\n*\t\t\tSUCCESS\r\n");
	return 0;
}

uint8_t reset(void) {
	printf("*Reseting i6 transmitter:\r\n");
	if(appReset()!=0) {
		printf("*\t\t\tERROR\r\n");
		return -1;
	}
	printf("*\t\t\tSUCCESS\r\n");
	return 0;
}


