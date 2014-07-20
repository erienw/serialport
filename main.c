#include <windows.h>
#include "resource.h"
#include "rs232.h"

/*----------------------------------------------------------------------------*/
HWND 		hWndDlg;
HWND 		hWndCBPort, hWndCBBR, hWndCBDB, hWndCBP, hWndCBSB, hWndCBFC;
HWND		hWndLBRDATA, hWndLBMSDATA, hWndEDTIN, hWndBTNCS, hWndEDTTI;
HWND		hWndPort;
/*----------------------------------------------------------------------------*/
/* dialog box message processing function declarations */
LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);

/*----------------------------------------------------------------------------*/
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_FIRST), hWndDlg, (DLGPROC)(DlgProc));
	return TRUE;
}
/*----------------------------------------------------------------------------*/
LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		{
			initport(hWndDlg, Msg, wParam, lParam);
			showdate(hWndDlg);
			return FALSE; // setfocus by ourself
		}
		break;	// WM_INITDIALOG
	case WM_COMMAND: // Windows Controls processing
		{
			switch(LOWORD(wParam)) // This switch identifies the control
			{
			case IDC_CB_PORTS:
				cbport(hWndDlg, Msg, wParam, lParam);
				break; // case IDC_CB_PORTS
			case IDC_CB_BAUDRATE:
				cbbaudrate(hWndDlg, Msg, wParam, lParam);
				break; // case IDC_CB_BAUDRATE
			case IDC_CB_DATABITS:
				cbdatabits(hWndDlg, Msg, wParam, lParam);
				break; // case IDC_CB_DATABITS
			case IDC_CB_PARITY:
				cbparity(hWndDlg, Msg, wParam, lParam);
				break; // case IDC_CB_PARITY
			case IDC_CB_STOPBITS:
				cbstopbits(hWndDlg, Msg, wParam, lParam);
				break; // case IDC_CB_STOPBITS
			case IDC_CB_FLOWCONTROL:
				cbflowcontrol(hWndDlg, Msg, wParam, lParam);
				break; // case IDC_CB_FLOWCONTROL
			case IDC_EDT_NUMBER:
				timerinterval(hWndDlg);
				break; // case IDC_EDT_NUMBER
			case IDC_BTN_OPEN:
				btnopen(hWndDlg);
				break; // case IDC_BTN_OPEN
			case IDC_BTN_CLOSE:
				btnclose(hWndDlg);
				break; // case IDC_BTN_CLOSE
			case IDC_BTN_CLEARSENT:
				btnclearsent(hWndDlg);
				break; // case IDC_BTN_CLEARSENT
			case IDC_BTN_CLEARMSENT:
				btnclearmsent();
				break; // case IDC_BTN_CLEARMSENT
			case IDC_BTN_CLEARRECEIVED:
				btnclearreceived();
				break; // case IDC_BTN_CLEARRECEIVED
			case IDC_BTN_RESET:
				btnreset(hWndDlg, Msg, wParam, lParam);
				break; // case IDC_BTN_RESET
			case IDC_BTN_EXIT:
				btnexit(hWndDlg);
				break; // case IDC_BTN_EXIT
			case IDM_SAVE:
				savedata();
				break; // IDM_SAVE
			case IDM_EXIT:
				btnexit(hWndDlg);
				break; // IDM_EXIT
			case IDM_ABOUT:
				MessageBox(NULL, TEXT("Author: thanh nguyen \n Email: thanhng1985@gmail.com"), TEXT("About"), MB_OK); 
				break; // IDM_ABOUT
				
			case IDCANCEL:
				btnexit(hWndDlg);
				break;
			default: 
				return FALSE;
				break;
			}
		}
		break; // WM_COMMAND
	case WM_TIMER:
		switch (wParam) 
		{ 
		case ID_UPDATE_TIMER: 
            infloop(hWndDlg); 
			showdate(hWndDlg);
			break; // ID_UPDATE_TIMER
		case ID_TIMER:
		default:
			showdate(hWndDlg);
			break; // default
		} 
		break; // WM_TIMER
	case WM_CLOSE:
		btnexit(hWndDlg);
		break; // case WM_CLOSE:

	default:
		return FALSE;
		break; // default
	}

	return TRUE;
}
