/*********************************************************************************
 * author: isold.wang@gmail.com
***********************************************************************************/

#include "standard_serialize.h"

CStandardSerialize::CStandardSerialize(char* apBuffer, int alBufLen, ENUM_TYPE abyType)
{
    mpBuffer = apBuffer;
    mbyType  = abyType;
    mlBufLen = alBufLen;
    mlDataLen = 0;
}

int CStandardSerialize::getDataLen()
{
    return mlDataLen;
}

#ifndef x86_64
int CStandardSerialize::Serialize(int64_t& aiValue)
{
    if(mlBufLen < (mlDataLen + sizeof(int64_t)))
        throw(-1);
        
    if(mbyType == LOAD) {
        memcpy(&aiValue, mpBuffer + mlDataLen, sizeof(int64_t));
    } else {
        memcpy(mpBuffer + mlDataLen, &aiValue, sizeof(int64_t));
    }
    
    mlDataLen += sizeof(int64_t);
    return 1;
}
#endif

//*****************************************************************************
//  ������  ���л���0��β���ַ�������
//  ������  char * apValue      �ַ�������
//          uint16_t awBufferLen    ���ɴ��ַ������ݵĻ�������С
//  ����ֵ��int  1= �ɹ��� -1 = ʧ��
//  �÷���
//*****************************************************************************
int CStandardSerialize::Serialize(char* apValue, uint16_t awMaxLen)
{
    if(mlBufLen < (mlDataLen + 2))
        throw(-1);
        
    uint16_t    lwLen = 0;
    
    if(mbyType == LOAD) {   //��ȡ
        //���ȶ�ȡ����
        memcpy(&lwLen, mpBuffer + mlDataLen, 2);
        mlDataLen += 2;
        
        //��ȡ���ݱ���
        if((lwLen >= awMaxLen) || ((mlDataLen + lwLen) > mlBufLen)) {
            throw(-1);
        }
        
        memcpy(apValue, mpBuffer + mlDataLen, lwLen);
        apValue[lwLen] = '\0';
        mlDataLen += lwLen;
    } else { //�洢
        //���ȴ洢����
        lwLen = strlen(apValue);
        
        if((lwLen >= awMaxLen) || (lwLen + mlDataLen + 2 > mlBufLen))
            throw(-1);
            
        memcpy(mpBuffer + mlDataLen, &lwLen, 2);
        mlDataLen += 2;
        //�洢���ݱ���
        memcpy(mpBuffer + mlDataLen, apValue, lwLen);
        mlDataLen += lwLen;
    }
    
    return 1;
}

//*****************************************************************************
//  ������  ���л�����
//  ������  char * apValue      ����
//          uint16_t& awLen         �����ݵ���������
//          uint16_t awBufferLen    ���ɴ����ݵĻ�������С
//  ����ֵ��int  1= �ɹ��� -1 = ʧ��
//  �÷���
//*****************************************************************************
int CStandardSerialize::Serialize(char* apValue, uint16_t awLen, uint16_t aiBufferLen)
{
    if((awLen > aiBufferLen) || (mlBufLen < (mlDataLen + awLen))) {
        TRACE(1, "CStandardSerialize::Serialize  <awLen>��" << awLen << " <aiBufferLen>��" << aiBufferLen << "<mlBufLen>��" << mlBufLen << "<mlDataLen>��" << mlDataLen);
        throw(-1);
    }
    
    if(mbyType == LOAD) {   //��ȡ
        //��Ϊ�ⲿ�ƶ��˶�ȡ���ȣ����Բ���Ҫ�����ݳ��Ƚ������л�
        memcpy(apValue, mpBuffer + mlDataLen, awLen);
    } else { //�洢���ݱ���
        memcpy(mpBuffer + mlDataLen, apValue, awLen);
    }
    
    mlDataLen += awLen;
    return 1;
}
//RSA �ӽ�����Ҫ
int CStandardSerialize::Serialize(uint8_t* apValue, uint16_t awLen)
{
    if(mlBufLen < (mlDataLen + awLen))
        throw(-1);
        
    if(mbyType == LOAD) {   //��ȡ
        //��Ϊ�ⲿ�ƶ��˶�ȡ���ȣ����Բ���Ҫ�����ݳ��Ƚ������л�
        memcpy(apValue, mpBuffer + mlDataLen, awLen);
    } else { //�洢���ݱ���
        memcpy(mpBuffer + mlDataLen, apValue, awLen);
    }
    
    mlDataLen += awLen;
    return 1;
}


