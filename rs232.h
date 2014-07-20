#ifndef RS232_H
#define RS232_H

#include <windows.h>

#define FALSE		0
#define TRUE		1
#define MAX_BYTE	16

typedef struct _portconfig
{
	char   	*portname;
	DWORD  	baudrate;
	DWORD  	databits;
	DWORD  	parity;
	DWORD	fParity;
	DWORD  	stopbits;
	DWORD  	flowcontrol;
	DWORD	fOutX;
	DWORD	fInX;
	DWORD 	fOutxDsrFlow;
	DWORD 	fOutxCtsFlow;
	DWORD	fDsrSensitivity ;
	DWORD 	fRtsControl;
	DWORD 	fDtrControl;
} PORTCONFIG;

#ifdef __cplusplus      // avoid c++ name mangling
  extern "C" {
#endif

void errorhandler(LPTSTR lpszFunction); 
int getport(void);
void initport(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
HWND openport(void);
DWORD readfromport(HWND hWndPort, char *buffer, int nBytesToRead);
DWORD writetoport(HWND hWndPort, char *buffer, int nBytesToWrite);
void closeport(HWND hWndPort);

LRESULT cbport(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT cbbaudrate(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT cbdatabits(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT cbparity(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT cbstopbits(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT cbflowcontrol(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
void timerinterval(HWND hWndDlg);

void btnopen(HWND hWndDlg);
void infloop(HWND hWndDlg);
void btnclose(HWND hWndDlg);
void btnclearsent(HWND hWndDlg);
void btnclearmsent(void);
void btnclearreceived(void);
void btnreset(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
void btnexit(HWND hWndDlg);
void showdate(HWND hWndDlg);
void enablecontrols(int bEnable);

#ifdef __cplusplus
  }
#endif 

#endif // RS232_H
