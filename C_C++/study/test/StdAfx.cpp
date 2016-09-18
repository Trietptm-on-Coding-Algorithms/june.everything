// stdafx.cpp : source file that includes just the standard includes
//	hook.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

LPWSTR ImageFile = L"\\image\\face.jpg";

/* ������־���� */
void DebugLog(const char *format, ...)
{
	va_list vl;
	FILE *pf = NULL;
	char szLog[512] = { 0, };

	va_start(vl, format);
	wsprintfA(szLog, format, vl);
	va_end(vl);

	OutputDebugStringA(szLog);
}

/* ����Ȩ�޺��� */
BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		DebugLog("OpenProcessToken error: %u\n", GetLastError());
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, // lookup privilege on local system
		lpszPrivilege,				// privilege to lookup 
		&luid))						// receives LUID of privilege
	{
		DebugLog("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
	{
		DebugLog("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		DebugLog("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

/* �����ҹ� */
BOOL hook_by_code(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew, PBYTE pOrgBytes)
{
	FARPROC pFunc = NULL;
	DWORD	dwOldProtect = 0, dwAddress = 0;
	BYTE	pBuf[5] = { 0xE9, 0, };
	PBYTE	pByte = NULL;
	HMODULE hMod = NULL;

	hMod = GetModuleHandleA(szDllName);
	if (hMod == NULL)
	{
		DebugLog("hook_by_code() : GetModuleHandle(\"%s\") failed!!! [%d]\n", szDllName, GetLastError());
		return FALSE;
	}

	pFunc = (FARPROC)GetProcAddress(hMod, szFuncName);
	if (pFunc == NULL)
	{
		DebugLog("hook_by_code() : GetProcAddress(\"%s\") failed!!! [%d]\n", szFuncName, GetLastError());
		return FALSE;
	}

	pByte = (PBYTE)pFunc;
	if (pByte[0] == 0xE9)
	{
		DebugLog("hook_by_code() : The API is hooked already!!!\n");
		return FALSE;
	}

	if (!VirtualProtect((LPVOID)pFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		DebugLog("hook_by_code() : VirtualProtect(#1) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	memcpy(pOrgBytes, pFunc, 5);

	dwAddress = (DWORD)pfnNew - (DWORD)pFunc - 5;
	memcpy(&pBuf[1], &dwAddress, 4);

	memcpy(pFunc, pBuf, 5);

	if (!VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect))
	{
		DebugLog("hook_by_code() : VirtualProtect(#2) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}

/* ж�عҹ� */
BOOL unhook_by_code(LPCSTR szDllName, LPCSTR szFuncName, PBYTE pOrgBytes)
{
	FARPROC pFunc = NULL;
	DWORD	dwOldProtect = 0;
	PBYTE	pByte = NULL;
	HMODULE hMod = NULL;

	hMod = GetModuleHandleA(szDllName);
	if (hMod == NULL)
	{
		DebugLog("unhook_by_code() : GetModuleHandle(\"%s\") failed!!! [%d]\n", szDllName, GetLastError());
		return FALSE;
	}

	pFunc = (FARPROC)GetProcAddress(hMod, szFuncName);
	if (pFunc == NULL)
	{
		DebugLog("unhook_by_code() : GetProcAddress(\"%s\") failed!!! [%d]\n", szFuncName, GetLastError());
		return FALSE;
	}

	pByte = (PBYTE)pFunc;
	if (pByte[0] != 0xE9)
	{
		DebugLog("unhook_by_code() : The API is unhooked already!!!");
		return FALSE;
	}

	if (!VirtualProtect((LPVOID)pFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		DebugLog("unhook_by_code() : VirtualProtect(#1) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	memcpy(pFunc, pOrgBytes, 5);

	if (!VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect))
	{
		DebugLog("unhook_by_code() : VirtualProtect(#2) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}

/*����Ƿ�Vista�Ժ��ϵͳ�汾*/
BOOL IsVistaLater()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	if (osvi.dwMajorVersion >= 6)
		return TRUE;
	return FALSE;
}


/* �Զ��崴��Զ���̺߳���
   ��Ҫ�Ǹ��ݲ�ͬϵͳ�汾��������Ӧ��Զ���߳�
*/
BOOL MyCreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf)
{
	HANDLE      hThread = NULL;
	FARPROC     pFunc = NULL;
	// Vista, 7, Server2008
	if (IsVistaLater())    
	{
		// GetProcAddress��������ָ���Ķ�̬���ӿ�(DLL)�е�����⺯����ַ��
		// GetModuleHandleA˵��:��ȡһ��Ӧ�ó����̬���ӿ��ģ����
		pFunc = GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateThreadEx");
		if (pFunc == NULL)
		{
			DebugLog("MyCreateRemoteThread() : GetProcAddress() failed!!! [%d]\n", GetLastError());
			return FALSE;
		}
		((PFNTCREATETHREADEX)pFunc)(&hThread, 0x1FFFFF,
			NULL, hProcess, pThreadProc, pRemoteBuf,
			FALSE, NULL, NULL, NULL, NULL);
		if (hThread == NULL)
		{
			DebugLog("MyCreateRemoteThread() : NtCreateThreadEx() failed!!! [%d]\n", GetLastError());
			return FALSE;
		}
	}
	else	// 2000, XP, Server2003
	{
		// CreateRemoteThread�Ǵ���һ�����������̵�ַ�ռ������е��߳�(Ҳ��:����Զ���߳�).��
		hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
		if (hThread == NULL)
		{
			DebugLog("MyCreateRemoteThread() : CreateRemoteThread() failed!!! [%d]\n", GetLastError());
			return FALSE;
		}
	}
	// Windows API���������ȴ����ڹ���״̬ʱ��������رգ���ô������Ϊ��δ����ġ�
	// �þ��������� SYNCHRONIZE ����Ȩ�ޡ�
	if (WAIT_FAILED == WaitForSingleObject(hThread, INFINITE))
	{
		DebugLog("MyCreateRemoteThread() : WaitForSingleObject() failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}

/**
 * Dllע�뺯��
 * @param dwPID Ҫע����̵�ID
 * @param szDllPath Ҫע���DLL·��
 * @ret ����ע���Ƿ�ɹ�
*/
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE                  hProcess = NULL;
	HANDLE                  hThread = NULL;
	LPVOID                  pRemoteBuf = NULL;
	DWORD                   dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE  pThreadProc = NULL;
	BOOL                    bRet = FALSE;
	HMODULE                 hMod = NULL;
	// 1. �򿪽���.OpenProcess ����������һ���Ѵ��ڵĽ��̶��󣬲����ؽ��̵ľ����
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		DebugLog("InjectDll() : OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
		goto INJECTDLL_EXIT;
	}
	// 2. ��������ڴ�ռ�.
	// VirtualAllocEx ��������������ָ�����̵�����ռ䱣�����ύ�ڴ����򣬳���ָ��MEM_RESET���������򽫸��ڴ�������0��
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	if (pRemoteBuf == NULL)
	{
		DebugLog("InjectDll() : VirtualAllocEx() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}
	// 3. ��DLLд��.�˺�����д��ĳһ���̵��ڴ����������������Է��ʣ����������ʧ�ܡ�
	if (!WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL))
	{
		DebugLog("InjectDll() : WriteProcessMemory() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}
	// 4. ��ȡһ��Ӧ�ó����̬���ӿ��ģ����
	hMod = GetModuleHandle("kernel32.dll");
	if (hMod == NULL)
	{
		DebugLog("InjectDll() : GetModuleHandle() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}
	// 5. GetProcAddress��������ָ���Ķ�̬���ӿ�(DLL)�е�����⺯����ַ
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");
	if (pThreadProc == NULL)
	{
		DebugLog("InjectDll() : GetProcAddress() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}
	// 6. CreateRemoteThread�Ǵ���һ�����������̵�ַ�ռ������е��߳�(Ҳ��:����Զ���߳�).��
	if (!MyCreateRemoteThread(hProcess, pThreadProc, pRemoteBuf))
	{
		DebugLog("InjectDll() : MyCreateRemoteThread() failed!!!\n");
		goto INJECTDLL_EXIT;
	}

	bRet = TRUE;

INJECTDLL_EXIT:

	if (pRemoteBuf)
		VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);

	if (hThread)
		CloseHandle(hThread);

	if (hProcess)
		CloseHandle(hProcess);

	return bRet;
}

/********************************
 * ��ȡ�ļ��Ĵ�С
 * @param char* path �ļ���·��
 * @ret long size �ļ��Ĵ�С
 ********************************/
unsigned long GetFileSize(LPWSTR path) 
{
	unsigned long filesize = -1L;
	FILE *fp = NULL;
	fp = _wfopen(path, L"r");
	if (fp == NULL)
		return filesize;
	fseek(fp,0L,SEEK_END);
	filesize = ftell(fp);
	fclose(fp);
	fp = NULL;
	return filesize;
}

/*****************************
 * �ַ������ˣ�ȥ�����з���
 * ע��ʹ����ɺ����ͷŵ�
 * @param data Դ����
 * @return ���˵����з�������
******************************/
char* filter(char* data)
{
	char *tp;
	unsigned int i = 0, j = 0;
	tp = (char*)calloc(strlen(data), sizeof(char));
	while (i < strlen(data)) {
		if (data[i] == '\n' || data[i] == '\r')
			i++;				//���ǵ��س��ͻ���
		else
			tp[j++] = data[i++];//�����¼������
	}
	tp[j] = '\0';	//����һ��������
	return tp;		//ע�������ʹ����ɺ�Ҫ�ͷŵ�
}

/*********************************
 * ��ȡ�ļ���base64�ַ���
 * @param char* path �ļ���·��
 * @retn char* �����ļ���Base64ֵ
 *********************************/
//char* GetBaes64(const char* path) 
char* GetBaes64(LPWSTR path) 
{
	ULONG usize = GetFileSize(path);
	FILE *fp = NULL;
	fp = _wfopen(path, L"rb");
	if (fp == NULL)
		return NULL;
	char* inBuffer = new char[usize];
	INT uReaded = fread(inBuffer, 1, usize, fp);
	const BYTE* pbBinary = (const BYTE *)inBuffer;//ͼƬ�Ķ���������

	INT needsize = BASE64_Encode(pbBinary,uReaded,NULL);
	//printf("Base64 needs size = %d\n",needsize);
	char *pCrypt1 = new char[needsize+1];
	BASE64_Encode(pbBinary,uReaded,pCrypt1);
	free(inBuffer);//�ͷŵ�inBuffer
	inBuffer = NULL; 
	fclose(fp);
	fp = NULL;
	return pCrypt1;//ʹ����ɺ����ͷŵ�
}

BOOL Save2File(char* datas,char* FileName)
{
	if(datas == '\0'){
		return FALSE;
	}
	FILE *fp = fopen(FileName, "w+");
	if (fp==0) {
		return FALSE;
	}
	fwrite(datas, sizeof(char)*(strlen(datas)), 1, fp);
	fclose(fp);
	return TRUE;
}

/*********************************
 * char��ת��wchar��
 * @param 
 * @ret 
**********************************/
WCHAR *char2wchar(char* data)
{
	size_t len = strlen(data)+1;
	size_t converted = 0;
	WCHAR *wStr = L"";
	//wStr = (WCHAR*)malloc(len*sizeof(WCHAR));
	//mbstowcs_s(&converted,wStr,len,data,_TRUNCATE);
	return wStr;
}
/****************************
 *	��ȡ��ǰ���̵�����Ŀ¼
 * @ret LPWSTR 
 ****************************/
LPWSTR GetPath() 
{
	setlocale(LC_CTYPE, "chs");		//����֧�����Ŀ� #include <locale.h>
	LPWSTR data = NULL;
	DWORD dwRet;
	int size = GetCurrentDirectory(0, NULL);
	data = (LPWSTR)malloc(size*sizeof(LPWSTR));
	dwRet = GetCurrentDirectoryW(size, data);
	//DebugLog("FilePath : %S\n", data);
	return data;
}

/**************************************
*	��ȡ��ǰ���̵�����Ŀ¼�µ��ļ���ַ
* @ret LPWSTR
***************************************/
LPWSTR GetImageFilePath() 
{
	//���ַ���ӡ��wprintf
	return GetImageFilePath(ImageFile);
}

LPWSTR GetImageFilePath(LPWSTR ImggeName) 
{
	//���ַ���ӡ��wprintf
	return lstrcatW(GetPath(), ImggeName);
}

/**************************************
* wchar�ַ�ת����char���ַ�����
* @param wchar_t * 
* @ret char *
***************************************/
char* WcharToChar(const wchar_t* wp)
{
	char *m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}

/**************************************
* char�ַ�ת����wchar���ַ�����
* @param char *
* @ret wchar_t *
***************************************/
wchar_t* CharToWchar(const char* c)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, c, strlen(c), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

/*******************************************************
* ���ݽ������ƻ�ȡָ��������Ϣ
* ע����ñ�֤�������Ƶ�Ψһ��
* @param HANDLE hProcess		���̾��
* @param HANDLE hModule			ģ����
* @param PROCESSENTRY32* pinfo	����
* @param MODULEENTRY32* minfo	ģ��
* @param LPWSTR processName		���̵�����
* @ret LPWSTR					�����ҵ�������Ϣ����ַ
********************************************************/
LPWSTR GetProcessInfo(HANDLE hProcess, HANDLE hModule, PROCESSENTRY32* pinfo, MODULEENTRY32* minfo, LPWSTR processName)
{
	BOOL report;			//�Ƿ����������־
	LPWSTR shortpath = L""; //����·������
	shortpath = (LPWSTR)malloc(MAX_PATH*sizeof(LPWSTR));//����Ҫ���������ڴ�ռ�
	report = Process32First(hProcess, pinfo);			//��ȡ��һ��������Ϣ
	while (report)
	{
		hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pinfo->th32ProcessID);	//�Խ���ID����ģ����Ϣ
		Module32First(hModule, minfo);													//��ȡÿһ��ģ����Ϣ
		GetShortPathNameW((const unsigned short *)minfo->szExePath, shortpath, 256);	//�����ҵ���·����ֵ������shortpath
		if (lstrcmpW((const unsigned short *)pinfo->szExeFile, processName) == 0)		//ָ���Ľ���
		{
			//DebugLog("shortpath:%S", shortpath);
			break;
		}
		report = Process32Next(hProcess, pinfo); //����������һ������
	}
	return shortpath;
}

/***************************************************
 * ��ȡ��ǰ���̵ĵ�ַ��ע��˵�ַ�Ǳ�����ͼƬ���Ƶ�
 * @param	LPWSTR processName ��������
 * @ret		LPWSTR ���ص�ǰ���̵���Ϣ��ַ
****************************************************/
LPWSTR GetCurrentProcessPath(LPWSTR processName)
{
	HANDLE hModule = NULL; //ģ����
	LPWSTR path = L"";
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //��ʼ�����̿���
	PROCESSENTRY32* pinfo = new PROCESSENTRY32;		//������Ϣ����
	pinfo->dwSize = sizeof(PROCESSENTRY32);			//�����ռ��С
	MODULEENTRY32* minfo = new MODULEENTRY32;		//ģ����Ϣ����
	minfo->dwSize = sizeof(MODULEENTRY32);			//�����ռ��С

	path = GetProcessInfo(hProcess, hModule, pinfo, minfo, processName); //���崦��

	CloseHandle(hProcess);	//�رս��̾�� 
	CloseHandle(hModule);	//�ر�ģ����
	//���˵��ļ�����
	lstrcpynW(path, path, lstrlen((const char *)path) - lstrlen((const char *)processName) + 1);
	path = lstrcatW(path, ImageFile);//�����µ�ͼƬ·��
	//DebugLog("image_path:%S",path);
	return path;
}

