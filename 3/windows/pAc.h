#ifndef PAC_H
#define PAC_H

#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<tchar.h>
#include<ctime>

#define buffer_size 3

#define producer_num 2
#define consumer_num 3

#define produce_times 6
#define consume_times 4

#define empty _T("empty")
#define full _T("full")
#define mutex _T("mutex")

#define buffer _T("buffer")

#define P(S) WaitForSingleObject(S, INFINITE)
#define V(S) ReleaseSemaphore(S, 1, NULL)

typedef struct{
	int arr[buffer_size];
	int number;
	int count;
}bufferArr;

typedef struct {
	bufferArr bufferarr;
	HANDLE s_empty;
	HANDLE s_full;
	HANDLE s_mutex;
}shareBuffer;

//�����������ݿռ�
HANDLE MakeSharedFile()
{
	//�����ļ�ӳ�����
	HANDLE hMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,	//ʹ��ҳʽ��ʱ�ļ�
		NULL,	//Ĭ�ϵİ�ȫ��
		PAGE_READWRITE,	//��дȨ
		0,	//�ļ����ߴ� ��32λ
		sizeof(shareBuffer),	//�ļ����ߴ� ��32λ
		buffer	//����
	);

	if (hMapping != INVALID_HANDLE_VALUE)
	{
		//���ļ�ӳ���ϴ�����ͼ
		LPVOID pData = MapViewOfFile(
			hMapping,	//�ļ�ӳ�����Ķ���
			FILE_MAP_ALL_ACCESS,	//��ö�дȨ
			0,	//���ļ��Ŀ�ͷ�� ��32λ��ʼ
			0,	//���ļ��Ŀ�ͷ�� ��32λ��ʼ
			0	//ӳ�������ļ�
		);

		if (pData != NULL)
		{
			ZeroMemory(pData, sizeof(shareBuffer));	//�����ڴ�ռ䣬����0
		}

		UnmapViewOfFile(pData);	//�����ļ�ӳ�����
	}

	return hMapping;
}

int getRandomInt()
{
	srand(time(0));
	return rand() % 10;
}

void show(bufferArr bufferarr)
{
	printf("������������Ϊ:");
	if (bufferarr.number == 0) {
		printf("��\n");
	}
	else {
		for (int i = 0; i < bufferarr.number; i++)
		{
			printf("%d ", bufferarr.arr[i]);
		}
		printf("\n");
	}
}
#endif