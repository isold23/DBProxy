#include <execinfo.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <std::string.h>
#include "file_stream.h"

//�õ���ǰӦ�ó����·��
void CFileStream::GetAppPath(char* apPath, int aiLen)
{
    memset(apPath, 0, aiLen);
#ifdef WIN32
    GetModuleFileName(NULL, (LPSTR)apPath, aiLen);
#else
    char lcAppPath[256 + 1] = "";
    sprintf(lcAppPath, "/proc/self/exe");
    readlink(lcAppPath, apPath, aiLen - 1);
#endif
}

//����·��
void CFileStream::CreatePath(char* apPath)
{
#ifdef WIN32
    CreateDirectory(apPath, NULL);
#else
    //mkdir�����ĵڶ��������������ô����ļ��е�Ȩ�ޣ�
    //Ȩ�޲���8���ƣ�����Ϊ777��ʾ�����û������Բ鿴
    mkdir(apPath, 511);
#endif
}

