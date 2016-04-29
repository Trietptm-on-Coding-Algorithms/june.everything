// InjectDll.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"
#include "windows.h"
#include "tchar.h"

BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath) {
	HANDLE hProcess =NULL, hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath)+1)*sizeof(TCHAR);
	LPTHREAD_START_ROUTINE pThreadProc;
	// #1 ʹ��dwPID��ȡĿ����̾����
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))) {
		_tprintf(L"OpenProcess(%d) failed![%d]\n",dwPID,GetLastError());
		return FALSE;
	}
	// #2 ��Ŀ����̵��ڴ��з���dwBufSize��С���ڴ�ռ�
	pRemoteBuf = VirtualAllocEx(hProcess,NULL,dwBufSize,MEM_COMMIT,PAGE_READWRITE);
	// #3 ��Ҫע���dll·��д�������ڴ档
	WriteProcessMemory(hProcess,pRemoteBuf,(LPVOID)szDllPath,dwBufSize,NULL);
	// #4 ��ȡLoadLibraryW ��API��ַ
	hMod = GetModuleHandle(L"kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod,"LoadLibraryW");
	// #5 ��Ŀ������������߳�
	hThread = CreateRemoteThread(hProcess,NULL,0,pThreadProc,pRemoteBuf,0,NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}

int _tmain(int argc,TCHAR *argv[]) {
	if (argc != 3) {
		_tprintf(L"USAGE:%s pid dll_path\n",argv[0]);
		return 1;
	}
	// inject dll
	if (InjectDll((DWORD)_tstol(argv[1]), argv[2])) 
		_tprintf(L"InjectDll(\"%s\") success!\n",argv[2]);
	else
		_tprintf(L"InjectDll(\"%s\") failed!\n", argv[2]);
	
	return 0;
}