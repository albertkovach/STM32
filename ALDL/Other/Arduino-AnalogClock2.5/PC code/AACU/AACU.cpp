// Start Visual Studio and create a new project : Visual C++ Win32 Console Application
// You can use any IDE or Gcc compiler instead of Visual Studio
// It would be the same



#include "stdafx.h"      // auto generated stuffs of Visual C++, delete this line if using gcc
#include <windows.h>
#include <iostream>
#include <time.h>

#define tcstrlen(a)				_tcslen(a) / sizeof(TCHAR)
#define compareString			wcscmp
#define formatString  			swprintf
#define copyString(a,b)	  		swprintf(a,L"%s",b)

#define MAX_KEY_LENGTH 255
#define MAX_STRING_LENGTH 255
#define MAX_STRING	256
#define MAX_VALUE_NAME 16383
#define BAUDRATE 9600

BOOL _COM_busy = FALSE;

INT _sendchars(HANDLE serialHandle, CHAR* ch)
{
	CHAR cmd[32];
	DWORD bytes;
	strcpy(cmd,ch);
	WriteFile(serialHandle, &cmd, sizeof (cmd), &bytes, NULL);
	return bytes;
}

INT _readchars(HANDLE serialHandle, CHAR* ch)
{
	DWORD bytes=0;
	int char_count=0;
	
	BYTE BUFF[MAX_STRING];
	ZeroMemory(BUFF, MAX_STRING);
	
	// because analogclock stops for reading serial only after a second
	// it affects COM reply time on PC software.
	// So, keep reading until we get first char, we might have to wait for 1s
	// before we can get reply from the microcontroller
	// also timeout > 1s here is required in case we are trying to communicate 
	// with other COM device that doesn't reply at all which traps program in infinity loop
	
	int ms=1000;
	while (!bytes && ms>0) { ReadFile(serialHandle, &BUFF[0], 1, &bytes, NULL); Sleep(1), ms--;}
	ch[char_count]= BUFF[char_count];
	char_count += bytes;

	// get the first byte, trying to get more or until 255
	while (bytes && char_count < MAX_STRING_LENGTH)
	{
		ReadFile(serialHandle, &BUFF[char_count], 1, &bytes, NULL);
		ch[char_count]= BUFF[char_count];
		char_count +=bytes; // keep counting if data available
		//Sleep(1);
	}
	ch[char_count]=0; //add null character at the end or at the very last 256

	return bytes;
}

INT _sendchar(HANDLE serialHandle, CHAR ch)
{
	DWORD bytes;
	WriteFile(serialHandle, &ch, sizeof (ch), &bytes, NULL);
	return bytes;
}

INT _readchar(HANDLE serialHandle, CHAR &ch)
{
	DWORD bytes=0;
	INT char_count=0;
	BYTE BUFF[MAX_STRING];
	ZeroMemory(BUFF, MAX_STRING);
	
	int ms=1000; //timeout 1s
	while (!bytes && ms>0) {ReadFile(serialHandle, &BUFF[0], 1, &bytes, NULL); Sleep(1), ms--;}
	//ch[t]= BUFF[t];
	char_count += bytes;

	// get the first byte, trying to get more or until 255
	while (bytes && char_count < MAX_STRING_LENGTH)
	{
		ReadFile(serialHandle, &BUFF[char_count], 1, &bytes, NULL);
		//ch[t]= BUFF[t];
		char_count +=bytes; // keep counting if data available
		//Sleep(1);
	}
	
	// keep reading while data is still available
	while (bytes && char_count < MAX_STRING_LENGTH) 
	{
		ReadFile(serialHandle, &BUFF[char_count], 1, &bytes, NULL);
		char_count +=bytes; // keep counting if data available
		Sleep(1);
	}
	
	ch = BUFF[0]; //get first char only
	
	//but return the whole data count
	return char_count;
}

INT SEND_COM(INT &PORT_NO, CHAR * data_in, CHAR * data_out)
{
	HANDLE serialHandle;
	CHAR cmd[32], msg[MAX_STRING];
	TCHAR b32[32];
	ZeroMemory(cmd, sizeof(cmd));
	strcpy(cmd, data_in);
	formatString(b32,L"\\\\.\\COM%d\\",PORT_NO);
	serialHandle = CreateFile((LPCWSTR)b32, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	_COM_busy = TRUE;
	if (serialHandle) 
	{ 
		DCB serialParams = { 0 };
		serialParams.DCBlength = sizeof(serialParams);

		GetCommState(serialHandle, &serialParams);
		serialParams.BaudRate = BAUDRATE;	//119200 - default CBR_9600;
		serialParams.ByteSize = 8;	//8 bit;
		serialParams.StopBits = 0;	//ONESTOPBIT;
		serialParams.Parity = 0;	//NOPARITY;
		SetCommState(serialHandle, &serialParams);

		// Set timeouts
		COMMTIMEOUTS timeout = { 0 };
		timeout.ReadIntervalTimeout = 50;
		timeout.ReadTotalTimeoutConstant = 50;
		timeout.ReadTotalTimeoutMultiplier = 50;
		timeout.WriteTotalTimeoutConstant = 50;
		timeout.WriteTotalTimeoutMultiplier = 10;

		SetCommTimeouts(serialHandle, &timeout);

		_sendchars(serialHandle, cmd);
		//Sleep(100);
		_readchars(serialHandle, msg);
		strncpy(data_out, msg, sizeof msg);
	}
	else PORT_NO = 0; // clear port_no when usb is unplug
	CloseHandle (serialHandle);
	_COM_busy = FALSE;
	return 0;
}

BOOL TryCOM(HANDLE serialHandle)
{
	// Do some basic settings
	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);

	GetCommState(serialHandle, &serialParams);
	serialParams.BaudRate = BAUDRATE;	//119200 - default CBR_9600;
	serialParams.ByteSize = 8;	//8 bit;
	serialParams.StopBits = 0;	//ONESTOPBIT;
	serialParams.Parity = 0;	//NOPARITY;
	SetCommState(serialHandle, &serialParams);

	// Set timeouts
	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;

	SetCommTimeouts(serialHandle, &timeout);
	
	CHAR hello = 'c', msg;//, tstr[9];
	//ZeroMemory(tstr,9);
	//for (int i=0;i<9;i++) tstr[i]=NULL;
		
	_sendchar(serialHandle, hello);
	//Sleep(100);
	_readchar(serialHandle, msg);
	if (msg=='z') return 1;
	return 0;
}


INT ScanCOM()
{ 
	int COMPORT_NO=0;
	HKEY hKey;
	LONG dwRegOPenKey = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM\\"), 0, KEY_READ, &hKey);
	if(dwRegOPenKey != ERROR_SUCCESS) return 0;

	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys=0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 
	
	HANDLE	 serHandle;
	DWORD i, retCode; 

	TCHAR  achValue[MAX_VALUE_NAME]; 
	DWORD cchValue = MAX_VALUE_NAME; 

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(hKey, achClass, &cchClassName, NULL, &cSubKeys, &cbMaxSubKey, &cchMaxClass, 
		&cValues, &cchMaxValue, &cbMaxValueData, &cbSecurityDescriptor, &ftLastWriteTime); 

	// Enumerate the key values. 

	BYTE * buffer = new BYTE[cbMaxValueData];
	ZeroMemory(buffer, cbMaxValueData);

	if (cValues) 
	{
		for (i=0, retCode = ERROR_SUCCESS; i<cValues; i++) 
		{ 
			cchValue = MAX_VALUE_NAME; 
			achValue[0] = '\0'; 
			retCode = RegEnumValue(hKey, i, achValue, &cchValue, NULL, NULL,NULL,NULL);

			if (retCode == ERROR_SUCCESS ) 
			{ 
				DWORD lpData = cbMaxValueData;
				buffer[0] = '\0';
				LONG dwRes = RegQueryValueEx(hKey, achValue, 0, NULL, buffer, &lpData);
				//_tprintf(TEXT("(%d) %s : %s\n"), i+1, achValue, buffer); 

				TCHAR buffer2[32];
				formatString(buffer2,L"\\\\.\\%s",buffer);
				
				serHandle = CreateFile((LPCWSTR)buffer, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				int err = GetLastError();
				//Sleep(500); //get some delay for connection can be made
				// etc, use err instead...
				_COM_busy = TRUE;
				if (err ==ERROR_SUCCESS) 
				{ 
					if (TryCOM(serHandle)) 
					{
						//Sleep(100);
						TCHAR tc_comport_no[4]; 
						wcscpy(tc_comport_no, &buffer2[7]);
						COMPORT_NO = _wtoi(tc_comport_no);
					}
				}
				CloseHandle (serHandle);
				_COM_busy = FALSE;
			} 
		}
	}

	//delete [] buffer;
	return COMPORT_NO;
}


// int main()                 /* if using other compiler */
int _tmain(int argc, _TCHAR* argv[]) 
{
	printf("\n\n**************************************\n\n");
	printf("CzAnalogClock Time Updater\n\nStart scanning...\n");
	
	int comport = ScanCOM();
	if (comport) 
	{
		printf("Found CzAnalogClock at port COM%d\n", comport);

		char tm_str[25] = "u2015/7/24 01:57:01u"; // sample
		char str_out[MAX_STRING];
		time_t ti = time(0);   // get time now

		//format time
		struct tm * now = localtime( &ti );
		strftime(tm_str, sizeof(tm_str), "u%Y/%m/%d %Xu", now);

		SEND_COM(comport, tm_str , str_out);

		printf(str_out);
	}
	else { 
		printf("CzAnalogClock can't be found.\nExit.\n\n");
		system("pause");
	}
	
	//scan();
	
	return 0;
}