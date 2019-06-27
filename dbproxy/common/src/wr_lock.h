#ifndef _AUTO_WR_LOCK_H
#define _AUTO_WR_LOCK_H (1)
/*
  *file AutoWRlock.h
  *brief linux read and write autolock class
  *author isold.wang@gmail.com
  *version 0.1
  *date 2010.11.26
  */
/*******************************************************************************
 * include
 *******************************************************************************/
#ifdef WIN32

#else
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#endif
#include "define.h"
#include <string>

/*******************************************************************************
 * class/struct
 *******************************************************************************/
class CRWSection
{
public:
    CRWSection();
    ~CRWSection();
    //����д�ٽ���
    inline void EnterWrite()
    {
#ifdef WIN32
#else
        pthread_rwlock_wrlock(&mrwlock);
#endif
    }
    //������ٽ���
    inline void EnterRead()
    {
#ifdef WIN32
#else
        pthread_rwlock_rdlock(&mrwlock);
#endif
    }
    //�뿪��д�ٽ���
    inline void LeaveWR()
    {
#ifdef WIN32
#else
        pthread_rwlock_unlock(&mrwlock);
#endif
    }
public:
#ifdef WIN32

#else
    //Linuxƽ̨��д������
    pthread_rwlock_t mrwlock;
#endif
};

//�Զ�д��ʵ����
class CAutoWRLock
{
public:
    //���캯��
    CAutoWRLock(CRWSection& aSection);
    //��������
    ~CAutoWRLock();
private:
    CRWSection& mSection;
};
//�Զ�����ʵ����
class CAutoRDLock
{
public:
    //���캯��
    CAutoRDLock(CRWSection& aSection);
    //��������
    ~CAutoRDLock();
private:
    CRWSection& mSection;
};

#endif

