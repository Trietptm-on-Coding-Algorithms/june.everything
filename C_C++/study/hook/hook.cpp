#include "stdafx.h"
#include "hook.h"

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

LPCWSTR ImageDatas = {'\0'};			//�Ǿ�����Base64����������
LPWSTR FilePath = L"image\\face.jpg";	//�ļ�·��
BYTE g_pWCTMB[5] = { 0, };//hook5���ֽ�WideCharToMultiByte

/*********************************************************************
 *	�Զ�����ַ�ת������
 *	�������ܣ��ú���ӳ��һ��unicode�ַ�����һ�����ֽ��ַ�����
 *	����ԭ�ͣ�
 *	int WideCharToMultiByte(
 *		UINT CodePage, //ָ��ִ��ת���Ĵ���ҳ
 *		DWORD dwFlags, //��������ж���Ŀ��ƣ�����Ӱ��ʹ���˶������ţ��������������ַ�
 *		LPCWSTR lpWideCharStr, //ָ��Ҫת��Ϊ���ֽ��ַ����Ļ�����
 *		int cchWideChar, //ָ���ɲ���lpWideCharStrָ��Ļ��������ַ�����
 *		LPSTR lpMultiByteStr, //ָ����ձ�ת���ַ����Ļ�����
 *		int cchMultiByte, //ָ���ɲ���lpMultiByteStrָ��Ļ��������ֵ
 *		LPCSTR lpDefaultChar, //����һ������ת���Ŀ��ַ����������ʹ��pDefaultChar����ָ����ַ�
 *		LPBOOL pfUsedDefaultChar //������һ���ַ�����ת��Ϊ����ֽ���ʽ�������ͻ�����������ΪTRUE
 *	);
 *	˵����
 *	CodePage��ָ��ִ��ת���Ĵ���ҳ�������������Ϊϵͳ�Ѱ�װ����Ч���κδ���ҳ��������ֵ����Ҳ����ָ����Ϊ���������һֵ��
 *		CP_ACP��ANSI����ҳ��
 *		CP_MACCP��Macintosh����ҳ��
 *		CP_OEMCP��OEM����ҳ��
 *		CP_SYMBOL�����Ŵ���ҳ��42����
 *		CP_THREAD_ACP����ǰ�߳�ANSI����ҳ��
 *		CP_UTF7��ʹ��UTF-7ת����
 *		CP_UTF8��ʹ��UTF-8ת����
 *
 ******************************************************************/
int WINAPI HookWideCharToMultiByte(UINT CodePage,
	DWORD dwFlags,
	LPCWSTR lpWideCharStr,
	int cchWideChar,
	LPSTR lpMultiByteStr,
	int cchMultiByte,
	LPCSTR lpDefaultChar,
	LPBOOL pfUsedDefaultChar)
{
	int result = 0;//��ʾʧ��
	FARPROC pFunc = NULL;
	HMODULE hMod = NULL;

	// unhook
	if (!UnHookByCode("kernel32.dll", "WideCharToMultiByte", g_pWCTMB))
	{
		//DebugLog("HookWideCharToMultiByte() : UnHookByCode() failed!!!\n");
		return result;
	}

	// call original API
	hMod = GetModuleHandleW(L"kernel32.dll");
	if (hMod == NULL)
	{
		//DebugLog("HookWideCharToMultiByte() : GetModuleHandle() failed!!! [%d]\n", GetLastError());
		goto __ERROR_EXIT;
	}

	pFunc = GetProcAddress(hMod, "WideCharToMultiByte");
	if (pFunc == NULL)
	{
		//DebugLog("HookWideCharToMultiByte() : GetProcAddress() failed!!! [%d]\n", GetLastError());
		goto __ERROR_EXIT;
	}

	// DebugLog("[redirect] change datas \n");
	// OK ������жϾͱȽ������ˣ�����Ҫ��������ȽϿ���
	if (CodePage == 3 && dwFlags == 0 && cchWideChar == -1 && cchMultiByte > 10000 && cchMultiByte < 36000 && lpDefaultChar == NULL && pfUsedDefaultChar == NULL ) {
		WCHAR *wstr = L"wangjunwei";
		FilePath = GetCurrentProcessPath();
		//DebugLog("FilePath[%s]\n",FilePath);
		char* data = GetBaes64(FilePath);
		if (data == NULL)
		{
			lpWideCharStr = NULL;
			cchWideChar = wcslen(ImageDatas) + 1;
		}
		else {
			wstr = CharToWchar(data);
			ImageDatas = wstr;
			lpWideCharStr = ImageDatas;
			cchWideChar = wcslen(ImageDatas) + 1;
		}
		free(data);//���������ͷŵ�
		data = NULL;
	}
	else if (CodePage == 3 && dwFlags == 0 && wcslen(lpWideCharStr) <=2  && cchWideChar == -1 && lpMultiByteStr == NULL && cchMultiByte == 0 && lpDefaultChar == NULL && pfUsedDefaultChar == NULL)
	{
		WCHAR *wstr = L"wangjunwei";
		FilePath = GetCurrentProcessPath();
		//OutputDebugStringW(FilePath);
		char* data = GetBaes64(FilePath);
		if (data == NULL)
		{
			lpWideCharStr = NULL;
			cchWideChar = wcslen(ImageDatas) + 1;
		}
		else {
			wstr = CharToWchar(data);	//CharToWchar  char2wchar
			ImageDatas = wstr;
			lpWideCharStr = ImageDatas;
			cchWideChar = wcslen(ImageDatas) + 1;
		}
		free(data);						//���������ͷŵ�
		data = NULL;
	}
	result = ((WIDECHARTOMULTIBYTE)pFunc)(CodePage,
		dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr,
		cchMultiByte, lpDefaultChar, pfUsedDefaultChar);

__ERROR_EXIT:
	// hook WideCharToMultiByte() function 
	BOOL hookResult = HookByCode("kernel32.dll", "WideCharToMultiByte", (PROC)HookWideCharToMultiByte, g_pWCTMB);
	if (!hookResult)
	{
		DebugLog("HookWideCharToMultiByte() : HookByCode() failed!!!\n");
	}

	return result;
}

/* DLL����������� */
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	setlocale(LC_CTYPE, "chs");
	char szCurProc[MAX_PATH] = { 0, };
	char *p = NULL;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		//DebugLog("DllMain() : DLL_PROCESS_ATTACH\n");
		//GetModuleFileNameA(NULL, szCurProc, MAX_PATH);
		//p = strrchr(szCurProc, '\\');
	//	DebugLog("%s\n",p);
		HookByCode("kernel32.dll", "WideCharToMultiByte", (PROC)HookWideCharToMultiByte, g_pWCTMB);
		break;
	case DLL_PROCESS_DETACH:
		//DebugLog("DllMain() : DLL_PROCESS_DETACH\n");
		UnHookByCode("kernel32.dll", "WideCharToMultiByte", g_pWCTMB);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
