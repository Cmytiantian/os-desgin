#include "stdafx.h"

#include <cstdio>
#include <cstdlib>
#include <iostream> 
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <iomanip>
#pragma comment(lib,"Shlwapi.lib")
#define DIV 1024
using namespace std;

//��ʾ������ǣ��ñ�Ǳ�ʾ����Ӧ�ó�����ڴ���з��ʵ�����
inline bool TestSet(DWORD dwTarget, DWORD dwMask)
{
	return ((dwTarget &dwMask) == dwMask);
}

#define SHOWMASK(dwTarget,type) if(TestSet(dwTarget,PAGE_##type)){cout << "," << #type;}

void showProtection(DWORD dwTarget)
{
	//�����ҳ�汣������
	SHOWMASK(dwTarget, READONLY);
	SHOWMASK(dwTarget, GUARD);
	SHOWMASK(dwTarget, NOCACHE);
	SHOWMASK(dwTarget, READWRITE);
	SHOWMASK(dwTarget, WRITECOPY);
	SHOWMASK(dwTarget, EXECUTE);
	SHOWMASK(dwTarget, EXECUTE_READ);
	SHOWMASK(dwTarget, EXECUTE_READWRITE);
	SHOWMASK(dwTarget, EXECUTE_WRITECOPY);
	SHOWMASK(dwTarget, NOACCESS);
}

void getSystemInformation()
{	//�����鱾P286
	//���ϵͳ��Ϣ
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);

	cout << "-------------------System Information-------------------" << endl;
	//�����ڴ�ҳ��С
	cout << "Virtual memory page size: " << si.dwPageSize / DIV << "KB" << endl;
	//��С��ַ
	cout << "Minimum application address: 0x" << si.lpMinimumApplicationAddress << endl;
	//����ַ
	cout << "Maximum application address: 0x" << si.lpMaximumApplicationAddress << endl;
	cout << endl;
}

void getPerformanceInformation()
{	//https://docs.microsoft.com/zh-cn/windows/desktop/api/psapi/ns-psapi-_performance_information
	//��ô洢����Ϣ
	PERFORMANCE_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	if (GetPerformanceInfo(&pi, sizeof(pi)) == false)
	{
		cout << GetLastError() << endl;
		return;
	}
	cout << "-------------------Performence Information-------------------" << endl;
	//ҳ���С ���ֽ�Ϊ��λ
	cout << "The size of a page: " << pi.PageSize / DIV << "KB" << endl;
	//ϵͳ��ǰ�ύ��ҳ��
	cout << "The number of pages currently committed by the system is: " << pi.CommitTotal << endl;
	//ϵͳ�����ڲ���չ��ҳ�ļ���������ύ�ĵ�ǰ���ҳ��
	cout << "The current maximum number of pages: " << pi.CommitLimit << endl;
	//�ϴ�����������ύ״̬��ҳ��
	cout << "The maximum number of pages since the last system reboot: " << pi.CommitPeak << endl;
	//ʵ�������ڴ��� ��ҳΪ��λ
	cout << "The amount of actual physical memory: " << pi.PhysicalTotal << endl;
	//��ǰ���õ������ڴ��� ��ҳΪ��λ
	cout << "The amount of physical memory currently available: " << pi.PhysicalAvailable << endl;
	//ϵͳ�����ڴ��� ��ҳΪ��λ
	cout << "The amount of system cache memory:" << pi.SystemCache << endl;
	//��ҳ�ͷǷ�ҳ�ں˳��е�ǰ�ڴ���ܺ� ��ҳΪ��λ
	cout << "The sum of the memory currently in the paged and nonpaged kernel pools: " << pi.KernelTotal << endl;
	//��ǰ�ڷ�ҳ�ں˳��е��ڴ� ��ҳΪ��λ
	cout << "The memory currently in the paged kernel pool: " << pi.KernelPaged << endl;
	//��ǰ�ڷǷ�ҳ�ں˳��е��ڴ� ��ҳΪ��λ
	cout << "The memory currently in the nonpaged kernel pool " << pi.KernelNonpaged << endl;
	//��ǰ�򿪾��������
	cout << "The current number of open handles: " << pi.HandleCount << endl;
	//��ǰ�򿪽��̵�����
	cout << "The current number of processes: " << pi.ProcessCount << endl;
	//��ǰ���̵߳�����
	cout << "The current number of threads: " << pi.ThreadCount << endl;
	cout << endl;

}

void getMemoryInformation()
{	//����https://docs.microsoft.com/zh-cn/windows/desktop/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	//��ȡ�����ڴ���Ϣ
	if (GlobalMemoryStatusEx(&statex) == false)
	{
		cout << GetLastError() << endl;
		return;
	}
	cout << "-------------------Physical Memory Infromation-------------------" << endl;
	//�ڴ�ʹ��
	cout << "There is " << statex.dwLength << " percent of memory in use." << endl;
	//�����ڴ��С
	cout << "There are " << statex.ullTotalPhys / DIV << " total KB of physical memory." << endl;
	//�����ڴ��С
	cout << "There are " << statex.ullAvailPhys / DIV << " free KB of physical memory." << endl;
	//�ܵ�ҳ�ļ���С
	cout << "There are " << statex.ullTotalPageFile / DIV << " total KB of paging file." << endl;
	//����ҳ�ļ���С
	cout << "There are " << statex.ullAvailPageFile / DIV << " free KB of paging file." << endl;
	//�����ڴ��С
	cout << "There are " << statex.ullTotalVirtual / DIV << " total KB of virtual memory." << endl;
	//���õ������ڴ��С
	cout << "There are " << statex.ullAvailVirtual / DIV << " free  KB of virtual memory." << endl;
	cout << endl;
}

void getProcessInformation()
{
	PROCESSENTRY32 cp;
	cp.dwSize = sizeof(cp);
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //���̿���
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		cout << "Create snapshot error" << endl;
		return;
	}

	cout << "-------------------Process Information-------------------" << endl;

	bool bMore = Process32First(hProcess, &cp);
	while (bMore)
	{
		cout << "PID: " << cp.th32ParentProcessID << " Name: " << cp.szExeFile << endl;
		bMore = Process32Next(hProcess, &cp);
	}

	CloseHandle(hProcess);
}

void getProcessInfoByID()
{
	int PID;
	cout << "-------------------Get process information by ID-------------------" << endl;
	cout << "Enter PID:";
	cin >> PID;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	SYSTEM_INFO si;	//ϵͳ��Ϣ�ṹ
	ZeroMemory(&si, sizeof(si));	//��ʼ��
	GetSystemInfo(&si);	//���ϵͳ��Ϣ

	MEMORY_BASIC_INFORMATION mbi;	//���������ڴ�ռ�Ļ�����Ϣ�ṹ
	ZeroMemory(&mbi, sizeof(mbi));	//���仺���������ڱ�����Ϣ

									//ѭ������Ӧ�ó����ַ�ռ�
	LPCVOID pBlock = (LPVOID)si.lpMinimumApplicationAddress;
	while (pBlock < si.lpMaximumApplicationAddress)
	{
		//�����һ�������ڴ�����Ϣ
		if (VirtualQueryEx(
			hProcess,	//��صĽ���
			pBlock,		//��ʼλ��
			&mbi,		//������
			sizeof(mbi)) == sizeof(mbi))	//���ȵ�ȷ�ϣ����ʧ�ܷ���0
		{
			//�����Ľ�β���䳤��
			LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
			TCHAR szSize[MAX_PATH];
			//������ת�����ַ���
			StrFormatByteSize(mbi.RegionSize, szSize, MAX_PATH);

			//��ʾ���ַ�ͳ���
			cout.fill('0');
			cout << hex << setw(8) << (DWORD)pBlock << "-" << hex << setw(8) << (DWORD)pEnd << (strlen(szSize) == 7 ? "(" : "(") << szSize << ")";

			//��ʾ���״̬
			switch (mbi.State)
			{
			case MEM_COMMIT:
				printf("Commited");
				break;
			case MEM_FREE:
				printf("Free");
				break;
			case MEM_RESERVE:
				printf("Reserved");
				break;
			}

			//��ʾ����
			if (mbi.Protect == 0 && mbi.State != MEM_FREE)
			{
				mbi.Protect = PAGE_READONLY;
			}
			showProtection(mbi.Protect);

			//��ʾ����
			switch (mbi.Type)
			{
			case MEM_IMAGE:
				printf(", Image");
				break;
			case MEM_MAPPED:
				printf(", Mapped");
				break;
			case MEM_PRIVATE:
				printf(", Private");
				break;
			}

			//�����ִ�е�ӳ��
			TCHAR szFilename[MAX_PATH];
			if (GetModuleFileName(
				(HMODULE)pBlock,			//ʵ�������ڴ��ģ����
				szFilename,					//��ȫָ�����ļ�����
				MAX_PATH) > 0)				//ʵ��ʹ�õĻ���������
			{
				//��ȥ·������ʾ
				PathStripPath(szFilename);
				printf(", Module:%s", szFilename);
			}

			printf("\n");
			//�ƶ���ָ���Ի����һ����
			pBlock = pEnd;
		}
	}
}

int main()
{
	getSystemInformation();		//��ȡϵͳ��Ϣ
	getPerformanceInformation();	//��ȡ������Ϣ
	getMemoryInformation();		//��ȡ�洢����Ϣ
	getProcessInformation();	//��ȡ������Ϣ
	getProcessInfoByID();	//����PID��ȡ�ý��̵������ַ�ռ�͹�����
	return 0;
}
