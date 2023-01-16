#include<windows.h>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<windowsx.h>
#include <winnt.rh>
using namespace std;
#define PATH_SIZE 512
#define BUFSIZE 1024

/// <summary>
/// OldPathΪԴ�ļ�·�������ļ�����Ŀ¼
/// NewPathΪ�´������ļ�·��
/// </summary>
/// <param name="OldPath"></param>
/// <param name="NewPath"></param>
void CopyFile(char* OldPath, char* NewPath) {
	WIN32_FIND_DATA lpFindData;
	//��Դ�ļ�
	HANDLE hOldFile = CreateFile(
		OldPath,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	//�������ļ�
	HANDLE hNewFile = CreateFile(
		NewPath,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hNewFile != INVALID_HANDLE_VALUE && hOldFile != INVALID_HANDLE_VALUE) {
		char buffer[BUFSIZE];
		DWORD dwxfer(0);
		//��Դ�ļ�����
		while (true) {
			ReadFile(
				hOldFile,
				buffer,
				BUFSIZE,
				&dwxfer,
				NULL);
			//�����ļ���д
			WriteFile(
				hNewFile,
				buffer,
				dwxfer,
				&dwxfer,
				NULL);
			if (dwxfer != BUFSIZE) break;
		}
		//�޸��ļ�ʱ������
		SetFileTime(
			hNewFile,
			&lpFindData.ftCreationTime,
			&lpFindData.ftLastAccessTime,
			&lpFindData.ftLastWriteTime);
		SetFileAttributes(NewPath, lpFindData.dwFileAttributes);
	}
	CloseHandle(hNewFile);
	CloseHandle(hOldFile);
	hOldFile = INVALID_HANDLE_VALUE;
	hNewFile = INVALID_HANDLE_VALUE;
}

/// <summary>
/// �ж��Ƿ�Ϊ��Ŀ¼�ļ�
/// </summary>
/// <param name="lpFindData"></param>
/// <returns></returns>
BOOL IsChildDir(WIN32_FIND_DATA& lpFindData) {
	return (
		((lpFindData.dwFileAttributes &
			FILE_ATTRIBUTE_DIRECTORY) != 0) &&
		(lstrcmp(lpFindData.cFileName, __TEXT(".")) != 0) &&
		(lstrcmp(lpFindData.cFileName, __TEXT("..")) != 0));
}

/// <summary>
/// OldPathΪ�����Ƶ�Դ�ļ�Ŀ¼·��
/// NewPathΪ�´�����Ŀ¼·��
/// </summary>
/// <param name="OldPath"></param>
/// <param name="NewPath"></param>
void MyCp(char* OldPath, char* NewPath) {
	char old_path[PATH_SIZE], new_path[PATH_SIZE];
	WIN32_FIND_DATA lpFindData;
	lstrcpy(old_path, OldPath);
	lstrcpy(new_path, NewPath);
	strcat(old_path, "/*.*");
	HANDLE hFindData;
	BOOL hFound = 0;
	hFindData = FindFirstFile(old_path, &lpFindData);
	if (hFindData != INVALID_HANDLE_VALUE) {
		//������һ���ļ�
		while ((hFound = FindNextFile(hFindData, &lpFindData)) != 0) {
			//·��ƴ��
			lstrcpy(old_path, OldPath);
			lstrcpy(new_path, NewPath);
			lstrcat(old_path, "/");;
			lstrcat(new_path, "/");
			lstrcat(old_path, lpFindData.cFileName);
			lstrcat(new_path, lpFindData.cFileName);
			//�ж��ļ��Ƿ�ΪĿ¼�ļ�
			if (IsChildDir(lpFindData)) {
				printf("child directory:%s begain copy\n", lpFindData.cFileName);
				CreateDirectory(new_path, NULL);
				MyCp(old_path, new_path);
				//�޸���Ŀ¼ʱ������
				HANDLE hDirFile = CreateFile(
					new_path,
					GENERIC_WRITE | GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					FILE_FLAG_BACKUP_SEMANTICS,
					NULL);
				SetFileTime(
					hDirFile,
					&lpFindData.ftCreationTime,
					&lpFindData.ftLastAccessTime,
					&lpFindData.ftLastWriteTime);
				SetFileAttributes(new_path, lpFindData.dwFileAttributes);
				CloseHandle(hDirFile);
				hDirFile = INVALID_HANDLE_VALUE;
				printf("child directory:%s copy finished\n", lpFindData.cFileName);
			}
			else {
				CopyFile(old_path, new_path);
				cout << "Filename:" << lpFindData.cFileName << "copy finished" << endl;
			}
		}
	}
	else {
		cout << "find file error!" << endl;
		exit(-1);
	}
	CloseHandle(hFindData);
	hFindData = INVALID_HANDLE_VALUE;
}

/// <summary>
/// argv���մ������д���������������ִ�г��� ���ļ�·�� ���ļ�·��
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[]) {
	WIN32_FIND_DATA lpFindData, lpFindNewData;
	HANDLE hFindFile;
	//����������Ĳ���ֻ����3��,
	if (argc != 3) {
		cout << "input parameters are error!" << endl;
		exit(-1);
	}
	//����Դ�ļ�
	if ((hFindFile = FindFirstFile(argv[1], &lpFindData)) == INVALID_HANDLE_VALUE) {
		cout << "Finding source file error!" << endl;
		exit(-1);
	}
	//������Ŀ¼
	if (FindFirstFile(argv[2], &lpFindNewData) == INVALID_HANDLE_VALUE) {
		CreateDirectory(argv[2], NULL);
		cout << "Creating directory successfully" << endl;
	}
	MyCp(argv[1], argv[2]);
	HANDLE hDirFile = CreateFile(
		argv[2],
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);
	SetFileTime(
		hDirFile,
		&lpFindData.ftCreationTime,
		&lpFindData.ftLastAccessTime,
		&lpFindData.ftLastWriteTime);
	SetFileAttributes(argv[2], lpFindData.dwFileAttributes);
	CloseHandle(hDirFile);
	CloseHandle(hFindFile);
	hFindFile = INVALID_HANDLE_VALUE;
	hDirFile = INVALID_HANDLE_VALUE;
	cout << "copy finished!" << endl;
	return 0;
}
