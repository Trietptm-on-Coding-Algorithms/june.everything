// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
LPCWSTR ImageDatas = {'\0'};			//�Ǿ�����Base64����������
LPWSTR FilePath = L"\\image.jpg";	//�ļ�·��
LPWSTR ProcessName = L"study.exe";
BYTE g_pZWRT[5] = { 0, };
BYTE g_pICW[5] = { 0, };

void test(){
	WCHAR *wstr = L"wangjunwei";
	FilePath = GetImageFilePath(FilePath);
	//FilePath = GetCurrentProcessPath(ProcessName);
	printf("%S\n",FilePath);
	char* data = GetBaes64(FilePath);
	if (data == NULL)
	{
		return;
	}
	else {
		ImageDatas = CharToWchar(data);
		printf("%S\n",ImageDatas);
		//Save2File(ImageDatas,"dataW.text");
		//lpWideCharStr = ImageDatas;
		//cchWideChar = wcslen(ImageDatas) + 1;
	}
	free(data);//���������ͷŵ�
	data = NULL;
}

int main(int argc, char* argv[])
{
	test();
	//printf("Hello World!\n");
	LPWSTR path = GetImageFilePath();
	printf("%S\n",path);
	char * data = GetBaes64(path);
	printf("%s%s\n",data,"wangjunwei");
	Save2File(data,"data.text");
	//MessageBox(NULL,"ddd","ddd",MB_OK);
	return 0;
}

