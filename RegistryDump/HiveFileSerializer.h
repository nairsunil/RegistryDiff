#pragma once
#include "..\\stdafx.h"
#include "CommonConstructs.h"

#define FILENAMELEN     27

class CRegistryRecordBuilder
{
private:
    template<class T>
    static void PackageHelperFixed(T value, TCHAR**lppszBuffer){
        *((T *)*lppszBuffer) = value;
        *lppszBuffer += sizeof(T);
    }
    static void PackageHelperVar(TCHAR *lpszValue, int iLength, TCHAR** lppszBuffer);
    static TCHAR* PackageRecordInMemory(const RegistryRecord *pRecord, int *pRecLen);
public:
    static int ConstructRecordBuffer(const RegistryRecord *pRecord, TCHAR ** lppszRecordBuffer);
 };

class CHiveFileSerializer
{
private:
    HANDLE  m_hFileHandle;
    TCHAR*  m_lpszFileName;
    DWORD   m_dwDesirecdAccess;
    DWORD   m_dwSharedMode;
    DWORD   m_dwCreationDisposition;
    DWORD   m_dwAttributeFlags;

public:
    CHiveFileSerializer();
    CHiveFileSerializer(TCHAR *lpszFileName,
        DWORD dwAccess = GENERIC_WRITE,
        DWORD dwShareMode = 0,
        DWORD dwCreationDispostion = CREATE_NEW,
        DWORD dwAttributeFlags = FILE_ATTRIBUTE_NORMAL);
    ~CHiveFileSerializer();
        
private:
    bool GenerateFileName();
public:
    bool InitSerialzier();
    void operator <<(const RegistryRecord *pRecord);
};

