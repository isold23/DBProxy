#ifndef DEF_TIME_STAMP_H
#define DEF_TIME_STAMP_H

#include "include.h"
#include "pool.h"

#define RECORD_MAX_COUNT        65536
#define TIME_STAMP_SECOND       7   //7���ӳ�ʱ

//ʱ�����Ϣ�ṹ
class CRecordItem
{
public:
    uint32_t    mulIpAddr;  //IP��ַ
    uint16_t          mwPort;     //�˿�
    int         mlTime;     //ʱ��
    CRecordItem* mpNext;
    CRecordItem* mpPre;
};

//ʱ�����
class CTimeStamp
{
private:
    CRecordItem*         mpList[RECORD_MAX_COUNT];  //ʱ�����������
    CCriticalSection    mCriSection;                //����������ٽ���
    
    CPool<CRecordItem>  moRecordPool;               //ʱ��������
    
    uint32_t                mdwRepeatCount;             //ʱ������δ���ڵĴ���
    
public:
    CTimeStamp();
    ~CTimeStamp();
    
    //���ʱ����Ƿ����
    bool CheckTimeStamp(uint16_t awPackSerial, uint32_t aulIpAddr, uint16_t awPort);
    
    //�����������
    void Dump();
};

#endif
