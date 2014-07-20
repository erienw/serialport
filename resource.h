#ifndef RESOURCE_H
#define RESOURCE_H

#define	IDM_EXIT				1001
#define IDM_SAVE				1002
#define IDM_ABOUT               1100

#define IDD_DLG_FIRST 			101
#define	ID_UPDATE_TIMER			1
#define	ID_TIMER				2

#define IDC_CB_PORTS 			200
#define	IDC_CB_BAUDRATE			201
#define	IDC_CB_DATABITS			202
#define	IDC_CB_PARITY			203
#define IDC_CB_STOPBITS			204
#define IDC_CB_FLOWCONTROL		205

#define IDC_BTN_OPEN			300
#define IDC_BTN_CLOSE			301
#define IDC_BTN_CLEARSENT		302
#define IDC_BTN_CLEARRECEIVED	303
#define IDC_BTN_CLEARMSENT		304
#define IDC_BTN_RESET			305
#define IDC_BTN_EXIT			306
#define IDC_CT_SHOWDATE			307

#define IDC_EDT_INPUT			400
#define IDC_LB_RDATA			401
#define IDC_LB_MSDATA			402
#define IDC_EDT_NUMBER			403

#define IDC_STATIC        		-1
/*
When compiling a .rc file, resource.h is included, since this is the default setup for .rc files. Unfortunately, unless the included resource.h header has a newline at the end, you get the following error, where XX is simply the line number of the last line in the file:
.\resource.h(XX) : fatal error RC1004: unexpected end of file found

When Visual Studio auto-generates the resource.h file for you while making a .rc file, it includes a newline at the end, so this problem doesn't seem apparent until you edit the resource.h file yourself and do not end the file in a newline.

This problem is probably due to the parsing algorithm of the Resource Compiler Tool, which will blindly only look for newlines and gets confused when it sees the EOF character before a final newline. It's also quite probable that this is a problem for any header file included in the .rc file, not just resource.h. 
*/

#endif		// RESOURCE_H
