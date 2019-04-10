#include "stdafx.h"
#include "iostream"
#include <Windows.h>
#include "string"

using namespace std;

void copySingleFile(const char *file_source, const char *file_target)//���Ƶ����ļ�
{
	WIN32_FIND_DATA lpfindfiledata;
	//����ָ���ļ�·��
	HANDLE h_find = FindFirstFile(file_source,//�ļ���
		&lpfindfiledata);//���ݻ�����

	HANDLE h_source = CreateFile(file_source,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	HANDLE h_target = CreateFile(file_target,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	WIN32_FIND_DATA time_source;
	GetFileTime(h_source, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);

	LONG size = lpfindfiledata.nFileSizeLow - lpfindfiledata.nFileSizeHigh;

	DWORD wordbit;

	//�¿�����������������
	int *BUFFER = new int[size];
	//Դ�ļ�������
	ReadFile(h_source,
		BUFFER,
		size,
		&wordbit,
		NULL);
	//Ŀ���ļ�д����
	WriteFile(h_target,
		BUFFER,
		size,
		&wordbit,
		NULL);

	SetFileTime(h_target, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);

	CloseHandle(h_find);
	CloseHandle(h_source);
	CloseHandle(h_target);
}

void copyDir(const char *d_source, const char *d_target)
{
	WIN32_FIND_DATA lpfindfiledata;
	string f_source = d_source;
	string f_target = d_target;

	f_source.append("\\*.*");
	f_target.append("\\");

	HANDLE hfind = FindFirstFile(f_source.data(),//�ļ���
		&lpfindfiledata);//���ݻ�����
	if (hfind != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(hfind, &lpfindfiledata) != 0)//ѭ������FindFirstFile()�������������һ���ļ�
		{
			//������һ���ļ��ɹ�
			if ((lpfindfiledata.dwFileAttributes) == 16)//�ж��Ƿ�ΪĿ¼s
			{
				if ((strcmp(lpfindfiledata.cFileName, ".") != 0) && (strcmp(lpfindfiledata.cFileName, "..") != 0))
				{
					f_source.clear();
					f_source.append(d_source).append("\\").append(lpfindfiledata.cFileName);
					f_target.append(lpfindfiledata.cFileName);//׷���ļ�

					CreateDirectory(f_target.data(), NULL);//ΪĿ���ļ�����Ŀ¼
					copyDir(f_source.data(), f_target.data());//������Ŀ¼����

					HANDLE h_source = CreateFile(f_source.data(),
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_BACKUP_SEMANTICS,
						NULL);
					HANDLE h_target = CreateFile(f_target.data(),
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_BACKUP_SEMANTICS,
						NULL);

					WIN32_FIND_DATA time_source;
					GetFileTime(h_source, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);
					SetFileTime(h_target, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);

					f_source.clear();
					f_source.append(d_source).append("\\");
					f_target.clear();
					f_target.append(d_target).append("\\");
				}
			}
			else
			{
				f_source.clear();
				f_source.append(d_source).append("\\").append(lpfindfiledata.cFileName);
				f_target.append(lpfindfiledata.cFileName);//׷���ļ�

				copySingleFile(f_source.data(),f_target.data());//ֱ�ӵ����ļ����ƺ���

				f_source.clear();
				f_source.append(d_source).append("\\");
				f_target.clear();
				f_target.append(d_target).append("\\");
			}
		}
	}
	else
	{
		cout << "����ָ���ļ�·�����ļ�ʧ��!" << endl;
	}
}

int main(int argc,char* argv[])
{
	if (argc != 3) //�жϲ����Ƿ����
	{
		cout << "Error Arguments" << endl;
		return 0;
	}
	else
	{
		WIN32_FIND_DATA lpfindfiledata;
		if (FindFirstFile(argv[1], &lpfindfiledata) == INVALID_HANDLE_VALUE)
		{
			printf("����Դ�ļ�·��ʧ��!\n");
		}
		if (FindFirstFile(argv[2], &lpfindfiledata) == INVALID_HANDLE_VALUE)
		{
			CreateDirectory(argv[2], NULL);//ΪĿ���ļ�����Ŀ¼
		}
		copyDir(argv[1], argv[2]);

		HANDLE h_source = CreateFile(argv[1],
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL);
		HANDLE h_target = CreateFile(argv[2],
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL);

		WIN32_FIND_DATA time_source;
		GetFileTime(h_source, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);
		SetFileTime(h_target, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);
	}
	printf("�������!\n");
	return 0;
}