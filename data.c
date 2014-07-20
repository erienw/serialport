#include <stdio.h>
#include <stdlib.h>
#include "data.h"

/*----------------------------------------------------------------------------*/
static 					FILE *fileName = NULL;
extern char				**gwbuff, **grbuff;
extern long				*gmsTime; // ReadIntervalTimeout + interval
/*----------------------------------------------------------------------------*/

void initdata(void)
{
	int i, nrows, ncolumns;
	nrows = INDEXMAX;
	ncolumns = 16;
	
	/* initialize gwbuff */
	
	gwbuff = (char **)malloc(nrows * sizeof(char *));
	if(gwbuff == NULL)
	{
		MessageBox(NULL, TEXT("Memory could not be allocated"), TEXT("Error"), MB_OK);
		exit(1);
	}
	else
	{
		for(i = 0; i < nrows; i++)
		{
			/* use calloc to set allocated memory to zero */
			gwbuff[i] = (char *)calloc(ncolumns, sizeof(char));
			if(gwbuff[i] == NULL)
			{
				MessageBox(NULL, TEXT("Memory could not be allocated"), TEXT("Error"), MB_OK);
				exit(1);
			}
		}
	}
	
	grbuff = (char **)malloc(nrows * sizeof(char *));
	if(grbuff == NULL)
	{
		MessageBox(NULL, TEXT("Memory could not be allocated"), TEXT("Error"), MB_OK);
		exit(1);
	}
	else
	{
		for(i = 0; i < nrows; i++)
		{
			/* use calloc to set allocated memory to zero */
			grbuff[i] = (char *)calloc(ncolumns, sizeof(char));
			if(grbuff[i] == NULL)
			{
				MessageBox(NULL, TEXT("Memory could not be allocated"), TEXT("Error"), MB_OK);
				exit(1);
			}
		}
	}
	
	gmsTime = (long *)malloc(INDEXMAX * sizeof(long));
	if(gmsTime == NULL)
	{
		MessageBox(NULL, TEXT("Memory could not be allocated"), TEXT("Error"), MB_OK);
		exit(1);
	}
	else
	{
		for (i = 0; i < INDEXMAX; i++)
			gmsTime[i] = 0;
	}
}

/*----------------------------------------------------------------------------*/

void getdata(void)
{
	TCHAR Buffer[BUFSIZE];
	DWORD dwRet;
	
	dwRet = GetCurrentDirectory(BUFSIZE, Buffer);

	if(dwRet == 0)
	{
		MessageBox(NULL, TEXT("GetCurrentDirectory failed"), TEXT("Error"), MB_OK);
		exit(1);
	}
	if(dwRet > BUFSIZE)
	{
		MessageBox(NULL, TEXT("Oever buffer size"), TEXT("Error"), MB_OK);
		exit(1);
	}
	
	strncat(Buffer, "\\data", BUFSIZE - strlen(Buffer));
	CreateDirectory(Buffer, NULL);
	strncat(Buffer, "\\data.txt", BUFSIZE - strlen(Buffer));
	
	fileName = fopen(Buffer,"w");

	if(fileName == NULL)
	{
		MessageBox(NULL, TEXT("Failed to open file data.txt"), TEXT("Error"), MB_OK);
		exit(1);
	}
	
    fseek(fileName, 0, SEEK_SET); 
	fprintf(fileName, "%6s\t", "index");
	fprintf(fileName, "%6s\t", "time interval (ms)");
	fprintf(fileName, "%16s\t\t", "written data");
	fprintf(fileName, "%16s\t\n", "received data");
		
	int i = 0;
	for (i = 0; i < INDEXMAX; i++)
	{
		fprintf(fileName, "%6d\t\t", i);
		fprintf(fileName, "%ld\t\t\t\t", gmsTime[i]);
		fprintf(fileName, "%16s\t\t", gwbuff[i]);
		fprintf(fileName, "%16s\t\n", *(grbuff + i));
	}

	fclose(fileName);
}

/*----------------------------------------------------------------------------*/

void freedata(void)
{
	int i;
	for (i = 0 ; i < INDEXMAX ; i++ )
	{
		free(gwbuff[i]);
		free(grbuff[i]);
	}
	free(gwbuff);
	free(grbuff);
	free(gmsTime);
}

/*----------------------------------------------------------------------------*/
long gettimeinterval(LARGE_INTEGER *start, LARGE_INTEGER *end)
{
    LARGE_INTEGER Frequency, elapsed;

    QueryPerformanceFrequency(&Frequency); 
    elapsed.QuadPart = end->QuadPart - start->QuadPart;

    elapsed.QuadPart *= 1000;
    elapsed.QuadPart /= Frequency.QuadPart;

    return elapsed.QuadPart;
}
/*----------------------------------------------------------------------------*/
