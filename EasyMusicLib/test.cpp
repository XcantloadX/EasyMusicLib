#include <windows.h>
#pragma comment(lib, "winmm.lib")

HWND hwnd;
MCIDEVICEID MCIOpen(LPCTSTR strPath)
{
	MCI_OPEN_PARMS mciOP;
	DWORD opReturn;
	mciOP.lpstrDeviceType = NULL;
	mciOP.lpstrElementName = strPath;  //Set the .wav file name to open
	opReturn = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD)(LPVOID)&mciOP);
	if (!opReturn)
		return mciOP.wDeviceID;
	return -1;
}
DWORD MCISeek(MCIDEVICEID wDeviceID, int sec)
{
	MCI_SEEK_PARMS SeekParms;
	SeekParms.dwTo = (sec) * 1000;
	return mciSendCommand(wDeviceID, MCI_SEEK, MCI_TO, (DWORD)(LPVOID)&SeekParms);
}
DWORD MCIPlay(MCIDEVICEID wDeviceID)
{
	MCI_PLAY_PARMS mciPP;
	mciPP.dwCallback = (DWORD_PTR)hwnd;
	return mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)&mciPP);
}
DWORD MCIPlayFrom(MCIDEVICEID wDeviceID, int sec)
{
	MCI_PLAY_PARMS play;
	play.dwFrom = sec * 1000;//Play From sec*1000 ms
	return mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY | MCI_FROM, (DWORD)&play);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case MM_MCINOTIFY:
	{
		if (MCI_NOTIFY_SUCCESSFUL == wParam) //MCI_NOTIFY_SUCCESSFUL means that the song has been played successfully. 
		{
			//To Do

		}
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
int main1()
{
	//static const char* class_name = "DUMMY_CLASS";
	//WNDCLASSEX wx = {};
	//wx.cbSize = sizeof(WNDCLASSEX);
	//wx.lpfnWndProc = WndProc;        // function which will handle messages
	//wx.hInstance = GetModuleHandleA(NULL);
	//wx.lpszClassName = (LPCWSTR)class_name;
	//if (RegisterClassEx(&wx)) {
	//	hwnd = CreateWindowEx(0, (LPCWSTR)class_name, L"dummy_name", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
	//}

	//open device
	MCIDEVICEID wDeviceID = MCIOpen(L"d:\\1.wav");  //Save DeviceID
	DWORD opReturn;
	if (wDeviceID != -1)
	{
		//MCI_SET_PARMS mciSet;
		//mciSet.dwTimeFormat = MCI_FORMAT_MILLISECONDS;//set time format to milliseconds
		//opReturn = mciSendCommand(wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID)&mciSet);

		////set the position at 30s.
		//opReturn = MCISeek(wDeviceID, 30);

		////play
		//opReturn = MCIPlay(wDeviceID);

		opReturn = MCIPlayFrom(wDeviceID, 30);
	}
	system("pause");
	//HACCEL hAccelTable = LoadAccelerators(wx.hInstance, (LPCWSTR)class_name);
	//MSG msg;
	//while (GetMessage(&msg, nullptr, 0, 0))
	//{
	//	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//}
	return 0;
}