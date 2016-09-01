// Copyright Ian White 2016
//
// This file is part of FSi6_updater
//
// FSi6_updater is free software: you can redistribute it and/or modify
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

#ifndef _TYPES_H
#define _TYPES_H

// SEE: https://msdn.microsoft.com/en-us/library/aa383751(VS.85).aspx
typedef enum { FALSE, TRUE } BOOL;
typedef unsigned char			BYTE;
typedef unsigned long			DWORD;
typedef void *						HANDLE;
typedef unsigned long *		PDWORD;
typedef unsigned int			UINT;
typedef unsigned short		WORD;

// SEE: https://msdn.microsoft.com/en-us/library/windows/desktop/aa363195(v=vs.85).aspx
typedef struct _COMMTIMEOUTS {
  DWORD ReadIntervalTimeout;
  DWORD ReadTotalTimeoutMultiplier;
  DWORD ReadTotalTimeoutConstant;
  DWORD WriteTotalTimeoutMultiplier;
  DWORD WriteTotalTimeoutConstant;
} COMMTIMEOUTS, *LPCOMMTIMEOUTS;

typedef struct _DCB {
  DWORD DCBlength;
  DWORD BaudRate;
  DWORD fBinary  :1;
  DWORD fParity  :1;
  DWORD fOutxCtsFlow  :1;
  DWORD fOutxDsrFlow  :1;
  DWORD fDtrControl  :2;
  DWORD fDsrSensitivity  :1;
  DWORD fTXContinueOnXoff  :1;
  DWORD fOutX  :1;
  DWORD fInX  :1;
  DWORD fErrorChar  :1;
  DWORD fNull  :1;
  DWORD fRtsControl  :2;
  DWORD fAbortOnError  :1;
  DWORD fDummy2  :17;
  WORD  wReserved;
  WORD  XonLim;
  WORD  XoffLim;
  BYTE  ByteSize;
  BYTE  Parity;
  BYTE  StopBits;
  char  XonChar;
  char  XoffChar;
  char  ErrorChar;
  char  EofChar;
  char  EvtChar;
  WORD  wReserved1;
} DCB, *LPDCB;

#define CBR_110			110
#define CBR_300			300
#define CBR_600			600
#define CBR_1200		1200
#define CBR_2400		2400
#define CBR_4800		4800
#define CBR_9600		9600
#define CBR_14400		14400
#define CBR_19200		19200
#define CBR_38400		38400
#define CBR_57600		57600
#define CBR_115200	115200
#define CBR_128000	128000
#define CBR_256000	256000

#define DTR_CONTROL_DISABLE		0x00
#define DTR_CONTROL_ENABLE		0x01
#define DTR_CONTROL_HANDSHAKE	0x02

#define RTS_CONTROL_DISABLE		0x00
#define RTS_CONTROL_ENABLE		0x01
#define RTS_CONTROL_HANDSHAKE	0x02
#define	RTS_CONTROL_TOGGLE		0x03

#define	NOPARITY			0
#define	ODDPARITY			1
#define	EVENPARITY		2
#define	MARKPARITY		3
#define	SPACEPARITY		4

#define	ONESTOPBIT		0
#define	ONE5STOPBITS	1
#define	TWOSTOPBITS		2

#endif // _TYPES_H
