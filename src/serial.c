// Copyright Ian White 2016
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

#include <libserialport.h>
#include "serial.h"

// #undef _DEBUG

uint8_t openCom(const char *comPortId, DWORD baudrate, HANDLE *hdlr) {
	struct sp_port *port;

	#ifdef _DEBUG
	printf("[DEBUG] Connecting to %s at %d bauds : " , comPortId, (int)baudrate);
	#endif

	if (sp_get_port_by_name(comPortId, &port) == SP_OK) {
		#ifdef _DEBUG
		printf("Port: %s\n", sp_get_port_name(port));
		#endif
	} else {
		#ifdef _DEBUG
		printf("ERROR - unable to get port (%s)\r\n", comPortId);
		#endif
		return -1;
	}
	
	if(sp_set_baudrate(port, baudrate) == SP_OK) {
		#ifdef _DEBUG
		printf("Baud: %d\n", baudrate);
		#endif
	} else {
		#ifdef _DEBUG
		printf("ERROR - unable to set baud rate to %d (%s)\r\n", baudrate, comPortId);
		#endif
		return -2;
	}

	if(sp_open(port, SP_MODE_READ_WRITE) == SP_OK) {
		#ifdef _DEBUG
		printf("Connected to %s\n", comPortId);
		#endif
	} else {
		#ifdef _DEBUG
		printf("ERROR - unable to open port (%s)\r\n", comPortId);
		#endif
		return -3;
	}

	// if(!SetCommTimeouts(*hdlr, &g_cto) || !SetCommState(*hdlr, &g_dcb)) {
	// 	#ifdef _DEBUG
	// 	printf("ERROR - unable to set timeouts (%s)\r\n", comPortId);
	// 	#endif

	// 	CloseHandle(*hdlr);
	// 	return -2;  // setting timeouts error
	// }

	// empty the buffers
	if(sp_flush(port, SP_BUF_BOTH) != SP_OK) {
		#ifdef _DEBUG
		printf("ERROR - flushing buffers (%s)\r\n", comPortId);
		#endif
		sp_close(port);
		return -4;
	}

	hdlr = (HANDLE)port;

	#ifdef _DEBUG
	printf("OK\r\n");
	#endif

	return 0;
}

uint8_t closeCom(HANDLE *hdlr) {
	if(sp_close((struct sp_port*)hdlr) != SP_OK) {
		#ifdef _DEBUG
		printf("ERROR - unable to close port\r\n");
		#endif
		return 1;
	}
	return 0;
}

uint8_t flushCom(HANDLE *hdlr) {
	// empty the buffers
	if(sp_flush((struct sp_port*)hdlr, SP_BUF_BOTH) != SP_OK) {
		#ifdef _DEBUG
		printf("ERROR - flushing buffers (%s)\r\n", comPortId);
		#endif
		return -1;
	}
	return 0;
}

uint8_t sendb(uint8_t byte, HANDLE *hdlr) {
	if(!sp_blocking_write((struct sp_port*)hdlr, &byte, 1, MAX_WAIT_WRITE_MS)) {
			return -1;  // error
	}
	else {
		//printf("byte sent:\t0x%X\r\n",byte);
		return 0; // ok
	}
}

uint8_t sends(uint16_t shrt, HANDLE *hdlr) {
	sendb((uint8_t)(shrt & 0xFF), hdlr);
	sendb((uint8_t)((shrt>>8) & 0xFF), hdlr);
	return 0;
}

uint8_t sendi(uint32_t val, HANDLE *hdlr) {
	sendb((uint8_t)(val & 0xFF), hdlr);
	sendb((uint8_t)((val>>8) & 0xFF), hdlr);
	sendb((uint8_t)((val>>16) & 0xFF), hdlr);
	sendb((uint8_t)((val>>24) & 0xFF), hdlr);
	return 0;
}

uint8_t senda(uint8_t *array, uint16_t size, HANDLE *hdlr) {
	if(sp_blocking_write((struct sp_port*)hdlr, array, size, MAX_WAIT_WRITE_MS) != SP_OK) {
			return -1;  // error
	}
	else return 0; // ok
}

uint8_t readb(uint8_t *byte, HANDLE *hdlr) {
	if(sp_blocking_read((struct sp_port*)hdlr, byte, 1, MAX_WAIT_READ_MS) != SP_OK) {
		return -1;  // error
	}
	else return 0; // ok
}

uint8_t reads(uint16_t *shrt, HANDLE *hdlr) {
	*shrt = 0x0000;
	uint8_t byte;
	if(readb(&byte, hdlr) != 0) return -1;
	
	*shrt += byte;
	if(readb(&byte, hdlr) != 0) return -1;
	
	*shrt += (uint16_t)byte << 8;
	return 0;
}
