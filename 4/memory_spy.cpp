#include <Windows.h>
#include <iostream>
#include <Shlwapi.h>
#include <iomanip>
#pragma comment(lib, "shlwapi.lib")
using namespace std;

int main()
{
	MEMORYSTATUSEX statex; //
	statex.dwLength = sizeof(statex);
	//��ȡϵͳ�ڴ���Ϣ
	GlobalMemoryStatusEx(&statex);
	printf("-----------------------�ڴ���Ϣ-----------------------\n");
	//�ڴ�ʹ����
	printf("�����ڴ��ʹ����Ϊ:%ld%%\n", statex.dwMemoryLoad);
	//�����ڴ�
	printf("�����ڴ��������Ϊ: %.2fGB.\n", (float)statex.ullTotalPhys / 1024 / 1024 / 1024);
	//���������ڴ�
	printf("���õ������ڴ�Ϊ: %.2fGB.\n", (float)statex.ullAvailPhys / 1024 / 1024 / 1024);
	//�ύ���ڴ�����
	printf("�ܵĽ����ļ�Ϊ:%.2fGB.\n", (float)statex.ullTotalPageFile / 1024 / 1024 / 1024);
	//��ǰ���̿����ύ������ڴ���
	printf("���õĽ����ļ�Ϊ��%.2fGB.\n", (float)statex.ullAvailPageFile / 1024 / 1024 / 1024);
	//�����ڴ�
	printf("�����ڴ��������Ϊ��%.2fGB.\n", (float)statex.ullTotalVirtual / 1024 / 1024 / 1024);
	//���������ڴ�
	printf("���õ������ڴ�Ϊ��%.2fGB.\n", (float)statex.ullAvailVirtual / 1024 / 1024 / 1024);
	//�����ֶ�
	printf("�����ֶε�����Ϊ��%.2fByte.\n", statex.ullAvailExtendedVirtual);
	return 0;
}