#ifndef COMMON_TWO_LOCK_QUEUE_H_
#define COMMON_TWO_LOCK_QUEUE_H_

/**********************************************************************
author: isold.wang@gmail.com
description:
        1 ����stl ��listʵ��
        2 ��������������ȳ�����(β��ͷ��)
        3 ��2����,���ƶ�д��,ֻ��һ��������ͷ,һ��������β
        4 ���ĵ���������32���̲���ʱ��һ��������queue�������10% ����

***********************************************************************/
#include <pthread.h>

template <typename T>
class TwoLockQueue
{
public:
    TwoLockQueue()
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&HMutex, &attr);
        pthread_mutex_init(&TMutex, &attr);
        //��ʼ��list,����һ���ڱ�node,��ͷ��βָ�붼ָ����
        list.push_back(T());
        iHead = list.begin();
        iTail = list.end();
    }
    ~TwoLockQueue()
    {
        pthread_mutex_lock(&HMutex);
        list.erase(iHead);
        pthread_mutex_unlock(&HMutex);
        pthread_mutex_destroy(&HMutex);
        pthread_mutex_destroy(&TMutex);
    }
    
    void push(const T& t)
    {
        try {
            //��β��
            pthread_mutex_lock(&TMutex);
            //׷��Ԫ��
            list.push_back(t);
            //��ȡlist β������
            iTail = list.end();
            //��β��
            pthread_mutex_unlock(&TMutex);
        } catch(...) {
            //��β��
            pthread_mutex_unlock(&TMutex);
        }
    }
    
    bool pop(T& t)
    {
        //��ͷ��
        pthread_mutex_lock(&HMutex);
        //��ȡlistͷ������
        typename TList::iterator iNext = iHead;
        //��ȡҪ�������ݵĵ�����
        ++iNext;
        
        if(iNext != list.end()) {  //�ж�queue�Ƿ�Ϊ��
            //��ȡҪ��������
            t = *iNext;
            //�ͷ����õ�����
            list.erase(iNext);
            //��ͷ��
            pthread_mutex_unlock(&HMutex);
            return true;
        }
        
        //��ͷ��
        pthread_mutex_unlock(&HMutex);
        return false;
    }
    
    int size()
    {
        int count = 0;
        //��ͷ��
        pthread_mutex_lock(&HMutex);
        {
            //��β��
            pthread_mutex_lock(&TMutex);
            //ȡlist����
            count = list.size();
            //ȥ��ͷ���Ļ���
            count--;
            //��ͷ��
            pthread_mutex_unlock(&HMutex);
        }
        //��β��
        pthread_mutex_unlock(&TMutex);
        return count;
    }
    
    bool empty()
    {
        //��β��
        pthread_mutex_lock(&TMutex);
        
        if(iHead != iTail) {  //�ж�queue�Ƿ�Ϊ��,(��Ϊ��)
            //��ͷ��
            pthread_mutex_unlock(&TMutex);
            return false;
        }
        
        //��β��
        pthread_mutex_unlock(&TMutex);
        return true;
    }
private:
    typedef std::list<T> TList;
    TList list;
    pthread_mutex_t HMutex;
    // stl��ģ������Ƕ�׵�ʱ����Ҫtypename ��ʶ
    typename TList::iterator iHead;
    pthread_mutex_t TMutex;
    // stl��ģ������Ƕ�׵�ʱ����Ҫtypename ��ʶ
    typename TList::iterator iTail;
};

#endif

