#include <stdio.h>
#include <string.h>
#include "resource.h"
#include "rs232.h"
#include "data.h"

/*----------------------------------------------------------------------------*/
char 			*sPorts[16];   // 16 serial com port can be accepted.
PORTCONFIG 		*portcf;
extern HWND 	hWndCBPort, hWndCBBR, hWndCBDB, hWndCBP, hWndCBSB, hWndCBFC;
extern HWND		hWndLBRDATA, hWndLBMSDATA, hWndEDTIN, hWndBTNCS, hWndEDTTI;
extern HWND		hWndPort;
int				bConnect, bSavedata, bReset, bClose;
char			**gwbuff, **grbuff;
unsigned long 	kr, kw, kt;	// read, write, time indexes
unsigned long	interval; 	// windows updated time
long			*gmsTime;	// ReadIntervalTimeout + interval
/*----------------------------------------------------------------------------*/

void errorhandler(LPTSTR lpszFunction) 
{ 
    /* Retrieve the system error message for the last-error code */ 
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    /* Display the error message and exit the process */ 

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 64) * sizeof(TCHAR)); 
    sprintf((LPTSTR)lpDisplayBuf, TEXT("%s failed with error %d:\n%s"), lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    //ExitProcess(dw); 
}
/*----------------------------------------------------------------------------*/

int getport(void)
{
    int numOfPorts = 0;
    LONG Status;
    HKEY  hKey;
    DWORD dwIndex = 0;
    CHAR  Name[32];
    DWORD szName;
    UCHAR PortName[16];
    DWORD szPortName;
    DWORD Type;
	
	int z;
    for (z = 0; z < 16; z++)
        sPorts[z] = (char*)malloc(32 * sizeof(char));

    if( RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),
                     0,
                     KEY_READ,
                     &hKey) != ERROR_SUCCESS)
    return -1;

    do
    { 
       szName = sizeof(Name);
       szPortName = sizeof(PortName);     
       
       Status = RegEnumValue(hKey, 
                             dwIndex++, 
                             Name, 
                             &szName, 
                             NULL, 
                             &Type,
	                         PortName, 
                             &szPortName);
       
       if((Status == ERROR_SUCCESS) )
	   {
          strcpy(sPorts[numOfPorts],(char*)PortName);
	      numOfPorts++;
	   }
    } while((Status == ERROR_SUCCESS) );
    
    RegCloseKey(hKey);
    return numOfPorts;    
}

/*----------------------------------------------------------------------------*/
void initport(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	char const 		*sBaudrates[9] = {"600", "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};
	char const 		*sDataBits[4] = {"5", "6", "7", "8"};
	char const 		*sParity[5] = {"Even", "Odd", "None", "Mark", "Space"};
	char const 		*sStopBits[3] = {"1", "1.5", "2"};
	char const 		*sFlowControl[3] = {"None", "Xon/Xoff", "Hardware"};
	int numOfPorts;
	
	portcf = malloc(sizeof(PORTCONFIG));
	if(portcf == NULL)
	{
		MessageBox(NULL, TEXT("Memory could not be allocated"), TEXT("Error"), MB_ICONERROR);
		exit(1);
	}
	
	portcf->portname = (char *) malloc(32 * sizeof(char));
	
	if(portcf->portname == NULL)
	{
		MessageBox(NULL, TEXT("Memory could not be allocated"), TEXT("Error"), MB_ICONERROR);
		exit(1);
	}

	hWndCBPort = GetDlgItem(hWndDlg, IDC_CB_PORTS);
	if (hWndCBPort == NULL)
	{	
		MessageBox(NULL, TEXT("Invalid dialog box HWND for CB_PORTS"), TEXT("Error"), MB_ICONERROR);
	}
	else
	{
		if (getport() != -1)
		{
			numOfPorts = getport(); 	
		}
		int j;
		for (j = 0;j < numOfPorts; j++)
		{
			SendMessage(hWndCBPort, CB_ADDSTRING, 0, (LPARAM)sPorts[j]);
			free(sPorts[j]);
		}

		int pSelected = SendMessage(hWndCBPort, CB_SETCURSEL, 0, 0);
		SendMessage(hWndCBPort,CB_GETLBTEXT, pSelected, (LPARAM)portcf->portname);		
	}
	
	hWndCBBR = GetDlgItem(hWndDlg, IDC_CB_BAUDRATE);
	if (hWndCBBR == NULL)
	{	
		MessageBox(NULL, TEXT("Invalid dialog box HWND for CB_BAUDRATE"), TEXT("Error"), MB_ICONERROR);
	}
	else
	{
		int j;
		for (j = 0; j < 9; j++)
			SendMessage(hWndCBBR, CB_ADDSTRING, 0, (LPARAM)sBaudrates[j]);
		SendMessage(hWndCBBR, CB_SETCURSEL, 4, 0);
		portcf->baudrate = CBR_9600;
	}
	
	hWndCBDB = GetDlgItem(hWndDlg, IDC_CB_DATABITS);
	if (hWndCBDB == NULL)
	{	
		MessageBox(NULL, TEXT("Invalid dialog box HWND for CB_DATABITS"), TEXT("Error"), MB_ICONERROR);
	}
	else
	{
		int j;
		for (j = 0; j < 4; j++)
			SendMessage(hWndCBDB, CB_ADDSTRING, 0, (LPARAM)sDataBits[j]);
		SendMessage(hWndCBDB, CB_SETCURSEL, 3, 0);
		portcf->databits = 8;
	}
	
	hWndCBP = GetDlgItem(hWndDlg, IDC_CB_PARITY);
	if (hWndCBP == NULL)
	{	
		MessageBox(NULL, TEXT("Invalid dialog box HWND for CB_PARITY"), TEXT("Error"), MB_ICONERROR);
	}
	else
	{
		int j;
		for (j = 0; j < 5; j++)
			SendMessage(hWndCBP, CB_ADDSTRING, 0, (LPARAM)sParity[j]);
		SendMessage(hWndCBP, CB_SETCURSEL, 2, 0);
		portcf->parity = NOPARITY;
	}
	
	hWndCBSB = GetDlgItem(hWndDlg, IDC_CB_STOPBITS);
	if (hWndCBSB == NULL)
	{	
		MessageBox(NULL, TEXT("Invalid dialog box HWND for CB_STOPBITS"), TEXT("Error"), MB_ICONERROR);
	}
	else
	{
		int j;
		for (j = 0; j < 3; j++)
			SendMessage(hWndCBSB, CB_ADDSTRING, 0, (LPARAM)sStopBits[j]);
		SendMessage(hWndCBSB, CB_SETCURSEL, 0, 0);
		portcf->stopbits = ONESTOPBIT;
	}
	
	hWndCBFC = GetDlgItem(hWndDlg, IDC_CB_FLOWCONTROL);
	if (hWndCBFC == NULL)
	{	
		MessageBox(NULL, TEXT("Invalid dialog box HWND for CB_FLOWCONTROL"), TEXT("Error"), MB_ICONERROR);
	}
	else
	{
		int j;
		for (j = 0; j < 3; j++)
			SendMessage(hWndCBFC, CB_ADDSTRING, 0, (LPARAM)sFlowControl[j]);
		SendMessage(hWndCBFC, CB_SETCURSEL, 1, 0);
		portcf->flowcontrol = 0x00;	
	}
	
	hWndEDTTI = GetDlgItem(hWndDlg, IDC_EDT_NUMBER);
	if (hWndEDTTI == NULL)
	{	
		MessageBox(NULL, TEXT("Invalid dialog box HWND for IDC_EDT_NUMBER"), TEXT("Error"), MB_ICONERROR);
	}
	else
	{
		SetDlgItemInt(hWndDlg, IDC_EDT_NUMBER, 100, FALSE);
	}
	
	hWndLBRDATA = GetDlgItem(hWndDlg, IDC_LB_RDATA);
	if (hWndLBRDATA == NULL)
	{	
		MessageBox(NULL, TEXT("Invalid dialog box HWND for IDC_LB_RDATA"), TEXT("Error"), MB_ICONERROR);
	}
	
	hWndLBMSDATA = GetDlgItem(hWndDlg, IDC_LB_MSDATA);
	if (hWndLBMSDATA == NULL)
	{	
		MessageBox(NULL, TEXT("Invalid dialog box HWND for IDC_LB_MSDATA"), TEXT("Error"), MB_ICONERROR);
	}
	
	hWndEDTIN = GetDlgItem(hWndDlg, IDC_EDT_INPUT);
	if (hWndEDTIN == NULL)
	{	
		MessageBox(NULL, TEXT("Invalid dialog box HWND for IDC_EDT_INPUT"), TEXT("Error"), MB_ICONERROR);
	}
	
	hWndBTNCS = GetDlgItem(hWndDlg, IDC_BTN_CLEARSENT);
	if (hWndBTNCS == NULL)
	{	
		MessageBox(NULL, TEXT("Invalid dialog box HWND for IDC_BTN_CLEARSENT"), TEXT("Error"), MB_ICONERROR);
	}
	
	/* fix me: bConnect and hWndPort have the same function? */
	interval = 100; // ms, timer interval 
	hWndPort = INVALID_HANDLE_VALUE;
	initdata();
	bConnect = FALSE;
	bSavedata = FALSE;
	bReset = FALSE;
	bClose = FALSE;
	kr = 0;
	kw = 0;
	kt = 0;
	enablecontrols(TRUE);
	SetFocus(hWndEDTIN);
}
/*----------------------------------------------------------------------------*/

HWND openport(void)
{
	int error;
	DWORD  accessdirection = GENERIC_READ | GENERIC_WRITE;
	HWND hWndPort = CreateFile(portcf->portname,
		accessdirection,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);
	if (hWndPort == INVALID_HANDLE_VALUE) {
		errorhandler("CreateFile");
		error = -2;
	}
	DCB dcb = {0};
	memset(&dcb,0,sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(hWndPort, &dcb)) {
		errorhandler("GetCommState");
		error = -4;
	}
	dcb.BaudRate = portcf->baudrate;
	dcb.Parity = portcf->parity;
	dcb.fParity = portcf->fParity;
	dcb.ByteSize = portcf->databits;
	dcb.StopBits = portcf->stopbits;
	
	dcb.fOutxCtsFlow    = portcf->fOutxCtsFlow;
    dcb.fOutxDsrFlow    = portcf->fOutxDsrFlow;
	dcb.fDsrSensitivity = portcf->fDsrSensitivity;
    dcb.fDtrControl     = portcf->fDtrControl;
    dcb.fDsrSensitivity = 0;
    dcb.fRtsControl     = portcf->fRtsControl;
    dcb.fOutX           = portcf->fOutX;
    dcb.fInX            = portcf->fInX;
	dcb.fErrorChar      = 0;
    dcb.fBinary         = 1;
    dcb.fNull           = 0;
    dcb.fAbortOnError   = 0;
    dcb.wReserved       = 0;
    dcb.XonLim          = 2;
    dcb.XoffLim         = 4;
    dcb.XonChar         = 0x13;
    dcb.XoffChar        = 0x19;
    dcb.EvtChar         = 0;
	
	if(!SetCommState(hWndPort, &dcb)){
		errorhandler("SetCommState");
		error = -8;
	}
	COMMTIMEOUTS timeouts = {0};
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.ReadTotalTimeoutMultiplier = 20;
	timeouts.WriteTotalTimeoutConstant = 100;
	timeouts.WriteTotalTimeoutMultiplier = 20;
	
	if(!SetCommTimeouts(hWndPort, &timeouts)){
		errorhandler("SetCommTimeouts");
		error = -16;
	}
	if (!error)
	{
		closeport(hWndPort);
		hWndPort = INVALID_HANDLE_VALUE;
	}
	return hWndPort;
}
/*----------------------------------------------------------------------------*/
/* nBytesToRead: maximum number of bytes to be read*/
DWORD readfromport(HWND hWndPort, char *buffer, int nBytesToRead)
{
	if (bConnect)
	{
		DWORD dwBytesRead = 0;
		if(!ReadFile(hWndPort, buffer, nBytesToRead, &dwBytesRead, NULL))
			errorhandler("ReadFile");
		return dwBytesRead;
	}
	else
		closeport(hWndPort);
}
/*----------------------------------------------------------------------------*/
/* nBytesToWrite: number of bytes to be written to the file or device */
DWORD writetoport(HWND hWndPort, char *buffer, int nBytesToWrite)
{
	if (bConnect)
	{
		DWORD dwBytesWritten = 0;
		if(!WriteFile(hWndPort, buffer, nBytesToWrite, &dwBytesWritten, NULL))
			errorhandler("WriteFile");
		return dwBytesWritten;
	}
	else
		closeport(hWndPort);
}
/*----------------------------------------------------------------------------*/
void closeport(HWND hWndPort)
{
	bConnect = FALSE;
	kr = 0;
	kw = 0;
	kt = 0;
	enablecontrols(TRUE);
	CloseHandle(hWndPort);
}
/*----------------------------------------------------------------------------*/
LRESULT cbport(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	bConnect = FALSE;
	
	switch (HIWORD(wParam)) // find out what message it was
	{
	case CBN_SELCHANGE: // this message means that the list is about to display
	{
		if (getport() != -1)
		{	
			int portSelected;
			portSelected = SendMessage(hWndCBPort, CB_GETCURSEL, 0, 0);
			SendMessage(hWndCBPort,CB_GETLBTEXT, portSelected, (LPARAM) portcf->portname);
		}
	}
		break;
	default: return FALSE;
	}
	return TRUE;	
}
/*----------------------------------------------------------------------------*/
LRESULT cbbaudrate(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	bConnect = FALSE;
	int brSelected;
	char br[32];
	
	switch (HIWORD(wParam)) // find out what message it was
	{
	case CBN_SELCHANGE: // this message means that the list is about to display
		{
			brSelected = SendMessage(hWndCBBR, CB_GETCURSEL, 0, 0);
			SendMessage(hWndCBBR,CB_GETLBTEXT, brSelected, (LPARAM)br);
			switch ((int)br)
			{
			case 600:
				 portcf->baudrate = CBR_600;
				break;
			case 1200:
				 portcf->baudrate = CBR_1200;
				break;
			case 2400:
				 portcf->baudrate = CBR_2400;
				break;
			case 4800:
				 portcf->baudrate = CBR_4800;
				break;
			case 9600:
				 portcf->baudrate = CBR_9600;
				break;
			case 19200:
				 portcf->baudrate = CBR_19200;
				break;
			case 38400:
				 portcf->baudrate = CBR_38400;
				break;
			case 57600:
				 portcf->baudrate = CBR_57600;
				break;
			case 115200:
				 portcf->baudrate = CBR_115200;
				break;
			default: portcf->baudrate = CBR_9600;
			}
		}
		break;
	default: return FALSE;
	}
	return TRUE;
}
/*----------------------------------------------------------------------------*/
LRESULT cbdatabits(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	bConnect = FALSE;
	int dbSelected;
	
	switch (HIWORD(wParam)) // find out what message it was
	{
	case CBN_SELCHANGE: // this message means that the list is about to display
		dbSelected = SendMessage(hWndCBDB, CB_GETCURSEL, 0, 0);
		switch (dbSelected)
		{
		case 0:
			portcf->databits = 5;			
			break;
		case 1:
			portcf->databits = 7;			
			break;
		case 2:
			portcf->databits = 6;			
			break;
		case 3:
			portcf->databits = 8;			
			break;
		default: portcf->databits = 8;
		}
		
		break;
	default: return FALSE;
	}
	return TRUE;
}
/*----------------------------------------------------------------------------*/
LRESULT cbparity(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	bConnect = FALSE;
	int pSelected;
	
	switch (HIWORD(wParam)) // find out what message it was
	{
	case CBN_SELCHANGE: // this message means that the list is about to display
		pSelected = SendMessage(hWndCBP, CB_GETCURSEL, 0, 0);
		switch (pSelected)
		{
		case 0:
			portcf->parity = EVENPARITY;
			portcf->fParity = 1;
			break;
		case 1:
			portcf->parity = ODDPARITY;
			portcf->fParity = 1;
			break;
		case 2:
			portcf->parity = NOPARITY;
			portcf->fParity = 0;
			break;
		case 3:
			portcf->parity = MARKPARITY;
			portcf->fParity = 1;
			break;
		case 4:
			portcf->parity = SPACEPARITY;
			portcf->fParity = 1;
			break;
		default: 
			portcf->parity = NOPARITY;
			portcf->fParity = 0;
		}
		break;
	default: return FALSE;
	}
	return TRUE;
	
}
/*----------------------------------------------------------------------------*/
LRESULT cbstopbits(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	bConnect = FALSE;
	int sbSelected;
	
	switch (HIWORD(wParam)) // find out what message it was
	{
	case CBN_SELCHANGE: // this message means that the list is about to display
		sbSelected = SendMessage(hWndCBP, CB_GETCURSEL, 0, 0);
		switch (sbSelected)
		{
		case 0:
			portcf->stopbits = ONESTOPBIT;
			break;
		case 1:
			portcf->stopbits = ONE5STOPBITS;
			break;
		case 2:
			portcf->stopbits = TWOSTOPBITS;
			break;
		}
		break;
	default: return FALSE;
	}
	return TRUE;
}
/*----------------------------------------------------------------------------*/
LRESULT cbflowcontrol(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	bConnect = FALSE;
	int fcSelected;
	
	switch (HIWORD(wParam)) // find out what message it was
	{
	case CBN_SELCHANGE: // this message means that the list is about to display
		fcSelected = SendMessage(hWndCBFC, CB_GETCURSEL, 0, 0);
		switch (fcSelected)
		{
		case 0:
			portcf->fOutX = FALSE;
			portcf->fInX = FALSE;
			portcf->fOutxDsrFlow = FALSE;
			portcf->fOutxCtsFlow = FALSE;
			portcf->fDsrSensitivity = FALSE;
			portcf->fRtsControl = RTS_CONTROL_DISABLE;
			portcf->fDtrControl = DTR_CONTROL_DISABLE;
			break;
		case 1:
			portcf->fOutX = TRUE;
			portcf->fInX = TRUE;
			portcf->fOutxDsrFlow = FALSE;
			portcf->fOutxCtsFlow = FALSE;
			portcf->fDsrSensitivity = FALSE;
			portcf->fRtsControl = RTS_CONTROL_ENABLE;
			portcf->fDtrControl = DTR_CONTROL_ENABLE;
			break;
		case 2:
			portcf->fOutX = FALSE;
			portcf->fInX = FALSE;
			portcf->fOutxDsrFlow = TRUE;
			portcf->fOutxCtsFlow = TRUE;
			portcf->fDsrSensitivity = TRUE;
			portcf->fRtsControl = RTS_CONTROL_HANDSHAKE;
			portcf->fDtrControl = DTR_CONTROL_HANDSHAKE;
			break;
		default: 
			portcf->fOutX = FALSE;
			portcf->fInX = FALSE;
			portcf->fOutxDsrFlow = FALSE;
			portcf->fOutxCtsFlow = FALSE;
			portcf->fDsrSensitivity = FALSE;
			portcf->fRtsControl = RTS_CONTROL_DISABLE;
			portcf->fDtrControl = DTR_CONTROL_DISABLE;
		}
		break;
	default: return FALSE;
	}
	return TRUE;
}
/*----------------------------------------------------------------------------*/
void timerinterval(HWND hWndDlg)
{
	if (!bConnect) // input interval when port is not opening
	{
		int bSuccess;
		/*  translates the text into an unsigned integer value */
		int nTimes = GetDlgItemInt(hWndDlg, IDC_EDT_NUMBER, &bSuccess, FALSE);
		if(bSuccess)
		{
			interval = nTimes;
		}
		else
		{
			SendDlgItemMessage(hWndDlg, IDC_EDT_NUMBER, EM_SETSEL, 0, -1L); 
		}
	}
}
/*----------------------------------------------------------------------------*/
void btnopen(HWND hWndDlg)
{
	if (!bConnect)	// prevent press this button multiple times
	{
		hWndPort = openport();
		if (hWndPort != INVALID_HANDLE_VALUE)
		{
			UINT uResult = SetTimer(hWndDlg, ID_UPDATE_TIMER, interval, (TIMERPROC) NULL);
			if (uResult == 0) 
			{ 
				errorhandler("No timer is available."); 
			}
			else
			{
				bConnect = TRUE;
				bClose = FALSE;
				bSavedata = FALSE;
				bReset = FALSE;
				SetFocus(hWndEDTIN);
			}
		}
		else
		{
			bConnect = FALSE;
			closeport(hWndPort);
			hWndPort = INVALID_HANDLE_VALUE;
		}
	}
}

/*----------------------------------------------------------------------------*/
/* a char variable takes 1 byte in memory */
void infloop(HWND hWndDlg)
{
	int nPorts = getport();
	
	if (nPorts > 0) // check if unplug port when it is opening
	{
		char *writebuffer;
		char *readbuffer;
		int bReadySend; 
		int len = SendMessage(hWndEDTIN, WM_GETTEXTLENGTH, 0, 0);
		
		/* 
		 * len should be limited, because it will be sent to LB.
		 * depend LB style so that it has limit of max item and length of string.
		 * LBS_EXTENDEDSEL: itemmax = 4672; average string length =	15 including
		 * NULL terminator (C string)
		 */
	
		if ((len > 0) && (len < 15))
		{
			writebuffer = (char*) GlobalAlloc(GPTR, (len + 1) * sizeof(char));
			SendMessage(hWndEDTIN, WM_GETTEXT, (WPARAM)((len+1) * sizeof(char)), (LPARAM)writebuffer);
			bReadySend = 1;
		}
		else 
		{
			bReadySend = 0;
			btnclearmsent();
		}
	
		readbuffer = (char*) GlobalAlloc(GPTR, (MAX_BYTE + 1) * sizeof(char));
		
		if ((bConnect) && (hWndPort != INVALID_HANDLE_VALUE))
		{	
			int bytesWritten;
			int bytesRead;
			LARGE_INTEGER StartingTime, EndingTime;
			QueryPerformanceCounter(&StartingTime);
			if (bReadySend)
			{
				bytesWritten = writetoport(hWndPort, writebuffer, strlen(writebuffer));
				if (bytesWritten > 0)
				{
					SendMessage(hWndLBMSDATA, LB_ADDSTRING, 0, (LPARAM) writebuffer);
					if (kw < INDEXMAX)
					{
						strncpy(gwbuff[kw], writebuffer, (len + 1));
						kw++;
					}
					GlobalFree((HGLOBAL)writebuffer);
					
					int nCount = SendMessage(hWndLBMSDATA, LB_GETCOUNT, 0, 0);
					if (nCount > 10) // limit number of items in LB
					{
						SendMessage(hWndLBMSDATA, LB_DELETESTRING, 0, 0);
					}
				}
			}
			bytesRead = readfromport(hWndPort, readbuffer, MAX_BYTE);
			
			if (bytesRead > 0)
			{
				readbuffer[bytesRead] = 0; // NULL at the end of a C string
				SendMessage(hWndLBRDATA, LB_ADDSTRING, 0, (LPARAM) readbuffer);
				if (kr < INDEXMAX)
				{
					strncpy(grbuff[kr], readbuffer, (len + 1));
					kr++;
				}
				GlobalFree((HGLOBAL)readbuffer);
			
				int nCount = SendMessage(hWndLBRDATA, LB_GETCOUNT, 0, 0);
				if (nCount > 15) // limit number of items in LB
				{
					SendMessage(hWndLBRDATA, LB_DELETESTRING, 0, 0);
				}
			}
			
			QueryPerformanceCounter(&EndingTime);
			long lTime = gettimeinterval(&StartingTime, &EndingTime);
			
			if (kt < INDEXMAX)
			{
				gmsTime[kt] = lTime + interval;
				kt++;
			}
			enablecontrols(FALSE);
		}
		else
		{
			closeport(hWndPort);
		}
	}
	else // if port is unpluged then close port and kill timer
	{
		bConnect = FALSE;
		KillTimer(hWndDlg, ID_UPDATE_TIMER);
		closeport(hWndPort);
	}
}
/*----------------------------------------------------------------------------*/
void btnclose(HWND hWndDlg)
{
	if (!bClose) // // prevent press this button multiple times
	{
		KillTimer(hWndDlg, ID_UPDATE_TIMER);	
		closeport(hWndPort);
		bConnect = FALSE;
		bSavedata = FALSE;
		bReset = FALSE;
		SetFocus(hWndEDTIN);
	}

	bClose = TRUE;
}
/*----------------------------------------------------------------------------*/
void btnclearsent(HWND hWndDlg)
{

	//SetWindowText(hWndBTNCS, ""); 
	SetDlgItemText(hWndDlg, IDC_EDT_INPUT, "");
	SetFocus(hWndEDTIN);
}
/*----------------------------------------------------------------------------*/
void btnclearmsent(void)
{
	SendMessage(hWndLBMSDATA, LB_RESETCONTENT, 0, 0);
}
/*----------------------------------------------------------------------------*/
void btnclearreceived(void)
{
	SendMessage(hWndLBRDATA, LB_RESETCONTENT, 0, 0);
}
/*----------------------------------------------------------------------------*/
/*
 * it is used to reinitialize port
 */
void btnreset(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (!bReset) // prevent press this button multiple times
	{
		if (bConnect = TRUE)
			bConnect = FALSE;
	
		SendMessage(hWndCBPort, CB_RESETCONTENT, 0, 0);
		SendMessage(hWndCBBR, CB_RESETCONTENT, 0, 0);
		SendMessage(hWndCBDB, CB_RESETCONTENT, 0, 0);
		SendMessage(hWndCBP, CB_RESETCONTENT, 0, 0);
		SendMessage(hWndCBSB, CB_RESETCONTENT, 0, 0);
		SendMessage(hWndCBFC, CB_RESETCONTENT, 0, 0);
	
		KillTimer(hWndDlg, ID_UPDATE_TIMER);
		btnclearsent(hWndDlg);
		btnclearmsent();
		btnclearreceived();
		closeport(hWndPort);
		initport(hWndDlg, Msg, wParam, lParam);
	}
	bReset = TRUE;
}
/*----------------------------------------------------------------------------*/
void btnexit(HWND hWndDlg)
{
	bConnect = FALSE;
	
	freedata();
	free(portcf->portname);
	free(portcf);
	
	KillTimer(hWndDlg, ID_UPDATE_TIMER);	
	KillTimer(hWndDlg, ID_TIMER);
	closeport(hWndPort);
	EndDialog(hWndDlg, 0);
}

/*----------------------------------------------------------------------------*/
void savedata(void)
{
	/* 
	 * prevent press this button multiple times and
	 * if not connecting and be closed
	 */
	if ((!bSavedata) && (!bConnect) && (bClose)) 
	{
		getdata();
		kr = kw = kt = 0;
	}
	bSavedata = TRUE;
	/* delete old data */
	freedata();
	/* initialize for next use */
	initdata();
}
/*----------------------------------------------------------------------------*/
void showdate(HWND hWndDlg)
{
	SetTimer (hWndDlg, ID_TIMER, 1000, NULL) ;
	
	SYSTEMTIME st;
	GetLocalTime(&st);
	char szDate[64];
	sprintf (szDate, "%04d/%02d/%02d - %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond);
	SetDlgItemText(hWndDlg, IDC_CT_SHOWDATE, szDate);
}
/*----------------------------------------------------------------------------*/
void enablecontrols(int bEnable)
{
	EnableWindow(hWndCBPort, bEnable);
	EnableWindow(hWndCBBR, bEnable);
	EnableWindow(hWndCBDB, bEnable);
	EnableWindow(hWndCBP, bEnable);
	EnableWindow(hWndCBSB, bEnable);
	EnableWindow(hWndCBFC, bEnable);
	EnableWindow(hWndEDTTI, bEnable);
}
/*----------------------------------------------------------------------------*/
