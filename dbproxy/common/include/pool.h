#ifndef _DEF_POOL_H_
#define _DEF_POOL_H_

#include "include.h"
#include "safelist.h"

template<class PooledType>
class CPool
{
public:
    CPool(int alMaxPooled = 2000);
    ~CPool();
    //�ӻ�����з�������
    PooledType* Malloc();
    //�ͷŵ���������
    void Free(PooledType* apObjet);
    //���������Ϣ
    void Dump();
private:
    CSafeList<PooledType> moPool;
    CCriticalSection moAccessCS;
    int mlCreateCount;
    int mlMaxPoolSize;
};

//�ӻ�����з�������
template< class PooledType >
PooledType* CPool<PooledType>::Malloc()
{
    PooledType* lpRetObj = NULL;
    
    //�ӻ�������ȡ
    if(moPool.GetCount()) {
        lpRetObj = moPool.DelHead();
    }
    
    if(lpRetObj) {
        return lpRetObj;
    } else {
        //����ò���ȥ����
        lpRetObj = new(std::nothrow) PooledType;
        
        if(lpRetObj) {
            CAutoLock loLock(moAccessCS);
            mlCreateCount++;
        }
        
        return lpRetObj;
    }
}

//�ӻ�����з�������
template< class PooledType >
void CPool<PooledType>::Free(PooledType* apObjet)
{
    ASSERT(apObjet);
    
    if(apObjet) {
        if(moPool.GetCount() < mlMaxPoolSize) {
            //�黹������
            moPool.AddTail(apObjet);
        } else {
            //ֱ��ɾ��
            delete apObjet;
            CAutoLock loLock(moAccessCS);
            mlCreateCount--;
        }
    }
}

//�ӻ�����з�������
template< class PooledType >
CPool<PooledType>::CPool(int alMaxPoolSize)
{
    ASSERT(alMaxPoolSize > 0);
    mlCreateCount = 0;
    mlMaxPoolSize = alMaxPoolSize;
}

//����
template< class PooledType >
CPool<PooledType>::~CPool()
{
    moPool.ClearAll();
}

//���������Ϣ
template< class PooledType >
void CPool<PooledType>::Dump()
{
    TRACE(1, "CPool::Dump ��������:" << mlCreateCount << ", ��󻺳�����:" << mlMaxPoolSize
          << ",��ǰ���е�Ԫ����:" << moPool.GetCount());
}
#endif //_DEF_POOL_H_
