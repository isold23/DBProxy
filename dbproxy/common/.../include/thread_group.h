#ifndef _THREAD_GROUP_H_
#define _THREAD_GROUP_H_

#include <list>
#include "include.h"
#include "critical_section.h"

//�̹߳�������
struct STRU_THREAD_CONTEXT;

//����ʹ�õ��̺߳���ԭ�ͺ������߳���Ҫ�ĺ���ԭ��
#ifdef WIN32
#include <process.h>
typedef unsigned(__stdcall ThreadFunc)(STRU_THREAD_CONTEXT&);
typedef unsigned(__stdcall StdThreadFunc)(void*);
#else
typedef unsigned(ThreadFunc)(STRU_THREAD_CONTEXT&);
typedef void* (StdThreadFunc)(void*);
#endif

//�ṩ�̵߳ļ��ӹ���
class CThreadStat
{

public:
    CThreadStat()
        : miStep(0)
    {
        memset(this, 0, sizeof(CThreadStat));
        Alive();
        threadIndex = 0;
    }
    
    //�㱨���
    void Alive()
    {
        ftime(&mtLastAliveTime);
        mui64AliveTimestamp = mtLastAliveTime.time * 1000 + mtLastAliveTime.millitm;
        ++muiAliveCounter;
    }
    
    //ȡ������
    int GetAliveCounter()
    {
        int liRet = muiAliveCounter;
        muiAliveCounter = 0;
        return liRet;
    }
    
    int GetAliveCounter(int)
    {
        return muiAliveCounter;
    }
    
    struct timeb GetLastAliveTime() const {
        return mtLastAliveTime;
    }
    
    int64_t GetAliveTimestamp() const
    {
        return mui64AliveTimestamp;
    }
    
    //�������ƺͱ��
    void SetThreadName(char* apThreadName, int aiNum = 0)
    {
        threadIndex = aiNum;
        
        if(apThreadName) {
            sprintf(macThreadName, "%d��%s", aiNum, apThreadName);
        }
    }
    
    //ȡ�߳�����
    char* GetThreadName()
    {
        return macThreadName;
    }
    
    //ȡ���ܲ���
    long GetPerformance()
    {
        long llTemp = mlPerformance;
        mlPerformance = 0;
        return llTemp;
    }
    
    //�����̵߳����ܲ���������ͳ�ƺ�ʱ����ִ�е�ʱ��
    void SetPerformance(long alPerformance)
    {
        if(mlPerformance < alPerformance) {
            mlPerformance = alPerformance;
        }
    }
    
    void SetStep(int n)
    {
        miStep = n;
    }
    
    int GetStep() const
    {
        return miStep;
    }
    
    int GetThreadIndex()
    {
        return threadIndex;
    }
    
private:
    uint32_t    muiAliveCounter;        // �̴߳�����
    int64_t           mui64AliveTimestamp;    // ���ʱ���
    struct timeb    mtLastAliveTime;        // ���һ�δ��ʱ��
    long            mlPerformance;          // �߳�����
    char            macThreadName[64 + 1];  // �߳�����
    int             miStep;
    int threadIndex;
};

class CThreadGroup;

struct STRU_THREAD_CONTEXT {
public:
    void*           mpWorkContext;      //�����Ļ���
    CThreadGroup*   mpGroupManager;     //������
    CThreadStat     moThreadStat;       //ͳ��
    int             miPriority;         //�̵߳����ȼ�
#ifdef WIN32
    HANDLE          mhHandle;           //�߳̾��
    uint32_t    muiThreadID;        //�߳�ID
#else
    timeb           moStartTime;        //����ʱ��
    pthread_t       muiThreadID;        //�߳�ID
#endif
};

//�߳���������
class CThreadGroup
{
public:
    //���졢����
    CThreadGroup(void);
    ~CThreadGroup(void);
    
public:
    //��ʼ�̵߳�����
    uint32_t Start(ThreadFunc* apThreadFucn, void* apWorkContext,
                   uint32_t auiThreadNum, char* apThreadName = NULL, int aiPriority = 0);
                   
    //�Ƿ�ֹͣ
    bool IsStop()
    {
        return mbStop;
    };
    //ֹͣ���߳�
    bool StopAll();
    
    //�͹����߷���
    bool Detach(STRU_THREAD_CONTEXT& apThread);
    //���������Ϣ
    void Dump();
    
    //���жϵĵȴ�
    bool Sleep(uint32_t ldwWaitTime);
    
    //���������߳�
    void WakeAll();
    
private:
    //�����߳�
    bool StartThread(STRU_THREAD_CONTEXT* apContext, ThreadFunc* apThreadFunc);
    //ֹͣ�߳�
    bool EndThread(STRU_THREAD_CONTEXT* apContext);
    
    //��ӡ�̵߳���Ϣ
    void PrintThreadInfo(STRU_THREAD_CONTEXT& aThreadContext);
    
#ifdef WIN32
    //������������߳�ʱ��
    void PrintThreadTime(char* apTime, FILETIME& aoFileTime);
#endif
    
private:
    //ֹͣ���
    bool                        mbStop;
    //�߳���Ϣ�б�
    std::list<STRU_THREAD_CONTEXT*>  moThreadList;
    //�ȴ�ʹ�õľ��
    CEvent                      moStopEvent;
    //�߳���Ϣ�ı���
    CCriticalSection            moAccessCS;
};
#endif //_THREAD_GROUP_H_
