#include "stdafx.h"
#include "iostream"
#include <Windows.h>
#include "string"

using namespace std;
void updateTime(const char* source, const char* target)
{//����Ŀ¼��ʱ��
	HANDLE h_source = CreateFile(source,	//���ļ��ȶ��������
		GENERIC_READ | GENERIC_WRITE,	//READ��WRITE����ʹ��ʵ�ֶ�д����
		FILE_SHARE_READ,	//��ʾ������ļ����й������
		NULL,	//Ĭ�ϰ�ȫ����
		OPEN_EXISTING,	//��һ���Ѿ����ڵ��ļ�������ļ������ڣ������ʧ��
		FILE_FLAG_BACKUP_SEMANTICS,	//ָʾϵͳΪ�ļ��Ĵ򿪻򴴽�ִ��һ�����ݻ�ָ�����
		NULL);
	HANDLE h_target = CreateFile(target,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);

	WIN32_FIND_DATA time_source;
	GetFileTime(h_source, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);
	SetFileTime(h_target, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);

	CloseHandle(h_source);
	CloseHandle(h_target);
}

void copySingleFile(const char *file_source, const char *file_target)//���Ƶ����ļ�
{
	WIN32_FIND_DATA lpfindfiledata;
	
	HANDLE h_find = FindFirstFile(file_source,//�ļ���
		&lpfindfiledata);//���ݻ�����

	HANDLE h_source = CreateFile(file_source,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,	
		FILE_ATTRIBUTE_NORMAL,	//Ĭ������
		NULL);

	HANDLE h_target = CreateFile(file_target,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,	//����һ�����ļ�������Ѵ�������ղ���д
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	WIN32_FIND_DATA time_source;
	GetFileTime(h_source, &time_source.ftCreationTime, &time_source.ftLastAccessTime, &time_source.ftLastWriteTime);

	LONG size = lpfindfiledata.nFileSizeLow - lpfindfiledata.nFileSizeHigh;	//��ȡ�ļ�����

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

	f_source.append("\\*.*");	//��ʾ���������ļ�

	HANDLE hfind = FindFirstFile(f_source.data(),//�ļ���
		&lpfindfiledata);//���ݻ�����
	if (hfind != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(hfind, &lpfindfiledata) != 0)//ѭ������FindFirstFile()�������������һ���ļ�
		{
			//������һ���ļ��ɹ�
			if (lpfindfiledata.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)//�ж��Ƿ�ΪĿ¼
			{
				if ((strcmp(lpfindfiledata.cFileName, ".") != 0) && (strcmp(lpfindfiledata.cFileName, "..") != 0))	//������ǵ�ǰ���Ŀ¼����һ��Ŀ¼
				{
					f_source.clear();
					f_source.append(d_source).append("\\").append(lpfindfiledata.cFileName);
					f_target.append("\\").append(lpfindfiledata.cFileName);//׷���ļ���

					CreateDirectory(f_target.data(), NULL);//ΪĿ���ļ�����Ŀ¼
					copyDir(f_source.data(), f_target.data());//������Ŀ¼����

					updateTime(f_source.data(), f_target.data());//����ʱ��

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
				f_target.append("\\").append(lpfindfiledata.cFileName);//׷���ļ�

				copySingleFile(f_source.data(), f_target.data());//ֱ�ӵ����ļ����ƺ���

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
	CloseHandle(hfind);
}

int main(int argc, char* argv[])
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
			cout<<"����Դ�ļ�·��ʧ��!"<<endl;
		}
		if (FindFirstFile(argv[2], &lpfindfiledata) == INVALID_HANDLE_VALUE)
		{
			CreateDirectory(argv[2], NULL);//ΪĿ���ļ�����Ŀ¼
		}

		copyDir(argv[1], argv[2]);	//��ʼ����

		updateTime(argv[1], argv[2]);	//����Ŀ¼��ʱ��
	}
	cout<<"�������!" << endl;
	return 0;
}