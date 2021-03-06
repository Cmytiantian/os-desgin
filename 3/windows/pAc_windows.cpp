// pAc.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<windows.h>
#include<iostream>
#include<pAc.h>
#include<string.h>
using namespace std;

void runProducer()
{
	//打开文件映射
	HANDLE hfileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, buffer);
	
	//打开信号量对象
	HANDLE s_empty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, empty);
	HANDLE s_full = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, full);
	HANDLE s_mutex = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, mutex);

	for (int i = 0; i < produce_times; i++)
	{
		//打开视图
		LPVOID pData = MapViewOfFile(hfileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		shareBuffer *sharebuffer = (shareBuffer*)pData;

		srand((unsigned)time(NULL));
		Sleep(1000);
		int data = rand() % 100 + 1;

		P(s_empty);
		P(s_mutex);

		//生产者操作
		int number = sharebuffer->bufferarr.number;
		sharebuffer->bufferarr.arr[number] = data;
		sharebuffer->bufferarr.number++;
		sharebuffer->bufferarr.count++;

		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("\n第%d次操作:\n", sharebuffer->bufferarr.count);
		printf("时间为: %02d : %02d : %02d . %03d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		printf("%ld 号生产者进程将 %d 放入buffer中\n",GetCurrentProcessId() ,data);
		printf("buffer的size为: %d\n", sharebuffer->bufferarr.number);

		show(sharebuffer->bufferarr);

		V(s_full);
		V(s_mutex);
	}
	UnmapViewOfFile(hfileMapping);
	CloseHandle(s_empty);
	CloseHandle(s_full);
	CloseHandle(s_mutex);
	CloseHandle(hfileMapping);
	
}

void runConsumer()
{
	//打开文件映射
	HANDLE hfileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, buffer);
	HANDLE s_empty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, empty);
	HANDLE s_full = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, full);
	HANDLE s_mutex = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, mutex);

	for (int i = 0; i < consume_times; i++)
	{
		
		//打开视图
		LPVOID pData = MapViewOfFile(hfileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		shareBuffer *sharebuffer = (shareBuffer*)pData;

		srand((unsigned)time(NULL));
		Sleep(1000);

		P(s_full);
		P(s_mutex);

		//消费者操作
		sharebuffer->bufferarr.arr[sharebuffer->bufferarr.number] = 0;
		sharebuffer->bufferarr.number--;
		sharebuffer->bufferarr.count++;
		int data = sharebuffer->bufferarr.arr[sharebuffer->bufferarr.number];

		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("\n第%d次操作:\n", sharebuffer->bufferarr.count);
		printf("时间为: %02d : %02d : %02d . %03d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		printf("%ld 号消费者进程将 %d 从buffer中取出\n",GetCurrentProcessId(), data);
		printf("buffer的size为: %d \n", sharebuffer->bufferarr.number);

		show(sharebuffer->bufferarr);

		V(s_empty);
		V(s_mutex);
	}
	UnmapViewOfFile(hfileMapping);
	CloseHandle(s_empty);
	CloseHandle(s_full);
	CloseHandle(s_mutex);
	CloseHandle(hfileMapping);
	
}

int main(int argc, char* argv[])
{
	if (argc != 1)
	{
		if (strcmp(argv[1], "p") == 0)
		{
			//运行生产者操作
			runProducer();
		}
		else if(strcmp(argv[1], "c") == 0)
		{
			//运行消费者操作
			runConsumer();
		}
		return 0;
	}

	HANDLE fileMapping = MakeSharedFile();
	//打开文件映射
	HANDLE hfileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, buffer);
	if (hfileMapping == NULL)
	{
		cout << "打开文件映射失败:" << GetLastError() << endl;
		return 0;
	}

	//创建视图
	LPVOID pData = MapViewOfFile(
		hfileMapping,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		0
	);

	if (pData == NULL)
	{
		cout << "创建视图失败:" << GetLastError() << endl;
		return 0;
	}
	else
	{
		shareBuffer *sharebuffer = (shareBuffer *)pData;
		sharebuffer->s_empty = CreateSemaphore(NULL, buffer_size, buffer_size, empty);
		sharebuffer->s_full = CreateSemaphore(NULL, 0, buffer_size, full);
		sharebuffer->s_mutex = CreateSemaphore(NULL, 1, 1, mutex);
		sharebuffer->bufferarr.number = 0;
		sharebuffer->bufferarr.count = 0;

		UnmapViewOfFile(pData);
		pData = NULL;
	}
	CloseHandle(hfileMapping);

	//创建生产者进程
	
	for (int i = 0; i < producer_num; i++)
	{
		TCHAR applicationName[MAX_PATH];
		TCHAR commandLine[MAX_PATH];
		GetModuleFileName(NULL, applicationName, MAX_PATH);//获取当前进程已加载模块的文件的完整路径
		sprintf_s(commandLine, "\"%s\" %s", applicationName, "p");
		STARTUPINFO si;
		memset(&si, 0, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pi;

		bool bRet = CreateProcess(
			applicationName,
			commandLine,
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL,
			&si, &pi
		);

		if (bRet) {
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}


	//创建消费者进程
	//sprintf_s(commandLine, "\"%s\" %s", applicationName, "c");

	for (int i = 0; i < consumer_num; i++)
	{
		TCHAR applicationName[MAX_PATH];
		TCHAR commandLine[MAX_PATH];
		GetModuleFileName(NULL, applicationName, MAX_PATH);//获取当前进程已加载模块的文件的完整路径
		sprintf_s(commandLine, "\"%s\" %s", applicationName, "c");
		STARTUPINFO si;
		memset(&si, 0, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pi;
		memset(&pi,0,sizeof(pi));

		bool bRet = CreateProcess(
			applicationName,
			commandLine,
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL,
			&si, &pi
		);

		if (bRet) {
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
	return 0;
}

