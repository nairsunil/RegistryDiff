////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	HiveFileSerializer.cpp
//
// summary:	Implements the hive file serializer class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "HiveFileSerializer.h"
#include <time.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Package helper variable. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="lpszValue">    [in,out] If non-null, the value. </param>
/// <param name="iLength">      Zero-based index of the length. </param>
/// <param name="lppszBuffer">  [in,out] If non-null, buffer for lppsz data. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CRegistryRecordBuilder::PackageHelperVar(TCHAR *lpszValue, int iLength, 
    TCHAR** lppszBuffer)
{
    memcpy((char *)*lppszBuffer, (const char *)lpszValue, iLength);
    *lppszBuffer += iLength;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Package record in memory. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="pRecord">  The record. </param>
/// <param name="pRecLen">  [in,out] If non-null, length of the record. </param>
///
/// <returns>   Null if it fails, else a pointer to a TCHAR. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

TCHAR* CRegistryRecordBuilder::PackageRecordInMemory(const RegistryRecord *pRecord, 
    int *pRecLen)
{
    TCHAR *lpszRecordBuffer, *lpszRecordPtr;
    int keyPathLen;

    keyPathLen = (int)(_tcslen(pRecord->lpszPathName) + 1);
    *pRecLen = keyPathLen + sizeof(int) + sizeof(int);

    lpszRecordBuffer = new(std::nothrow) TCHAR[*pRecLen];
    if (NULL == lpszRecordBuffer)
        return NULL;

    lpszRecordPtr = lpszRecordBuffer;

    CRegistryRecordBuilder::PackageHelperVar(pRecord->lpszPathName, keyPathLen, &lpszRecordPtr);
    CRegistryRecordBuilder::PackageHelperFixed<int>(pRecord->iSubKeyCount, &lpszRecordPtr);
    CRegistryRecordBuilder::PackageHelperFixed<int>(pRecord->iValueCount, &lpszRecordPtr);

    return lpszRecordBuffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Construct record buffer. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="pRecord">              The record. </param>
/// <param name="lppszRecordBuffer">    [in,out] If non-null, buffer for lppsz record data. </param>
///
/// <returns>   An int. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

int CRegistryRecordBuilder::ConstructRecordBuffer(const RegistryRecord *pRecord, 
    TCHAR ** lppszRecordBuffer)
{
    int iRecLength, iBufferLength = -1;
    TCHAR *lpszRecordBuffer, *lpszBufferPtr;

    lpszRecordBuffer = CRegistryRecordBuilder::PackageRecordInMemory(pRecord, &iRecLength);
    if (NULL == lpszRecordBuffer)
        return -1;

    iBufferLength = iRecLength + sizeof(int);
    *lppszRecordBuffer = new (std::nothrow) TCHAR[iBufferLength];
    if (*lppszRecordBuffer != NULL)
    {
        lpszBufferPtr = *lppszRecordBuffer;

        CRegistryRecordBuilder::PackageHelperFixed<int>(iRecLength, &lpszBufferPtr);
        CRegistryRecordBuilder::PackageHelperVar(lpszRecordBuffer, iRecLength, &lpszBufferPtr);
    }

    return iBufferLength;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Default constructor. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CHiveFileSerializer::CHiveFileSerializer()
{
    m_hFileHandle = INVALID_HANDLE_VALUE;
    m_lpszFileName = NULL;
    m_dwDesirecdAccess = GENERIC_WRITE;
    m_dwSharedMode = 0;
    m_dwCreationDisposition = CREATE_NEW;
    m_dwAttributeFlags = FILE_ATTRIBUTE_NORMAL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Constructor. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="lpszFileName">         [in,out] If non-null, filename of the file. </param>
/// <param name="dwAccess">             The access. </param>
/// <param name="dwShareMode">          The share mode. </param>
/// <param name="dwCreationDispostion"> The creation dispostion. </param>
/// <param name="dwAttributeFlags">     The attribute flags. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

CHiveFileSerializer::CHiveFileSerializer(TCHAR *lpszFileName,
    DWORD dwAccess,
    DWORD dwShareMode,
    DWORD dwCreationDispostion,
    DWORD dwAttributeFlags)
{
    m_lpszFileName = lpszFileName;
    m_dwDesirecdAccess = dwAccess;
    m_dwSharedMode = dwShareMode;
    m_dwCreationDisposition = dwCreationDispostion;
    m_dwAttributeFlags = dwAttributeFlags;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Destructor. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CHiveFileSerializer::~CHiveFileSerializer()
{
    if (m_lpszFileName) {
        delete[] m_lpszFileName;
        m_lpszFileName = NULL;
    }

    if (m_hFileHandle != INVALID_HANDLE_VALUE)
        CloseHandle(m_hFileHandle);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Generates a file name. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <returns>   True if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

bool CHiveFileSerializer::GenerateFileName()
{
    time_t t = time(0);
    struct tm now;
    localtime_s(&now, &t);

    m_lpszFileName = new (std::nothrow)TCHAR[FILENAMELEN + 1];
    if (m_lpszFileName == NULL)
    {
        printf("\n[CHiveFileSerializer::GenerateFileName] Error: Memory allocation error.");
        return false;
    }

    _stprintf_s(m_lpszFileName, FILENAMELEN, _T("RegDump_%02d%02d%02d%02d%02d%02d.rdmp"),
        now.tm_mday, now.tm_mon, now.tm_year, now.tm_hour, now.tm_min, now.tm_sec);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Initializes the serialzier. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <returns>   True if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

bool CHiveFileSerializer::InitSerialzier()
{
    if (m_lpszFileName == NULL)
    {
        if (!GenerateFileName())
        {
            return false;
        }
    }

    m_hFileHandle = CreateFile(m_lpszFileName, m_dwDesirecdAccess,m_dwSharedMode, 
        NULL, m_dwCreationDisposition, m_dwAttributeFlags, NULL);
    if (m_hFileHandle == INVALID_HANDLE_VALUE)
    {
        printf("\n[CHiveFileSerializer::InitSerialzier] Error: Failed to create the dump file (%d)", GetLastError());
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Bitwise left shift operator. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="pRecord">  The record. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CHiveFileSerializer::operator << (const RegistryRecord *pRecord)
{
    if (INVALID_HANDLE_VALUE == m_hFileHandle)
    {
        printf("\n[CHiveFileSerializer] Error creating dump file. Serializer not initialized.");
        return;
    }

    TCHAR *lpszBuffer = NULL;
    int nBufferLen = CRegistryRecordBuilder::ConstructRecordBuffer(pRecord, &lpszBuffer);
    if (nBufferLen > 0)
    {
        DWORD dwNumberOfBytesWritten = 0;
        if (WriteFile(m_hFileHandle, lpszBuffer, nBufferLen, &dwNumberOfBytesWritten, NULL) == FALSE) {
            //TODO
        }
        delete[] lpszBuffer;
    }
}
