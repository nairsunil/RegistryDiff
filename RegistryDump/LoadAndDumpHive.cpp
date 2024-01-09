    ////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	LoadAndDumpHive.cpp
//
// summary:	Implements the load and dump hive class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadAndDumpHive.h"
#include "RegistryHive.h"

#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   A macro that defines Maximum key length. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_KEY_LENGTH  255
using namespace std;

CRITICAL_SECTION CLoadAndDumpHive::m_cs;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Constructor. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="pFileSerializer">  [in,out] If non-null, the file serializer. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

CLoadAndDumpHive::CLoadAndDumpHive(CHiveFileSerializer* pFileSerializer)
    : m_pFileSerialzer(pFileSerializer)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Destructor. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CLoadAndDumpHive::~CLoadAndDumpHive()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Dumps a hive in file. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="recList">  [in,out] If non-null, list of records. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CLoadAndDumpHive::DumpHiveInFile(RegistryRecord* recList)
{
    while (recList != NULL)
    {
        *m_pFileSerialzer << recList;
        DumpHiveInFile(recList->childNodes);
        recList = recList->next;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Loads hive in memory. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="hRootKey">     Handle of the root key. </param>
/// <param name="lpszSubKey">   [in,out] If non-null, the sub key. </param>
/// <param name="currentNode">  [in,out] If non-null, the current node. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CLoadAndDumpHive::LoadHiveInMemory(HKEY hRootKey,
    TCHAR* lpszSubKey,
    RegistryRecord** currentNode)
{
    HKEY        hCurKey;
    DWORD       lResult, i, retCode;
    DWORD       cSubKeys = 0, cValues = 0;
    DWORD       cbName;
    TCHAR       achKey[MAX_KEY_LENGTH];

    lResult = RegOpenKeyEx(hRootKey, lpszSubKey, 0, KEY_READ, &hCurKey);

    lResult = RegQueryInfoKey(hCurKey,                // key handle 
        NULL,                   // buffer for class name 
        NULL,                   // size of class string 
        NULL,                   // reserved 
        &cSubKeys,              // number of subkeys 
        NULL,                   // longest subkey size 
        NULL,                   // longest class string 
        &cValues,               // number of values for this key 
        NULL,                   // longest value name 
        NULL,                   // longest value data 
        NULL,                   // security descriptor 
        NULL);                  // last write time 


    size_t keyLen = 10 + _tcsclen(_T("\\")) + _tcsclen(lpszSubKey);
    TCHAR *lpszKeyPath = new(std::nothrow) TCHAR[keyLen];
    if (lpszKeyPath == NULL)
        return;

    _stprintf_s(lpszKeyPath, keyLen, _T("%x\\%s"), (LONG)hRootKey, lpszSubKey);

    if (*currentNode == NULL)
    {
        *currentNode = new (std::nothrow) RegistryRecord();
        if (*currentNode != NULL)
        {
            size_t pathLen = _tcslen(lpszKeyPath) + 1;
            (*currentNode)->lpszPathName = new (std::nothrow)TCHAR[pathLen];
            if ((*currentNode)->lpszPathName != NULL)
            {
                _tcscpy_s((*currentNode)->lpszPathName, pathLen, lpszKeyPath);

                (*currentNode)->iValueCount = cValues;
                (*currentNode)->iSubKeyCount = cSubKeys;

                (*currentNode)->next = NULL;
                (*currentNode)->prev = NULL;
                (*currentNode)->childNodes = NULL;
                (*currentNode)->endPtr = &(*currentNode)->childNodes;
            }
            else
            {
                printf("[CLoadAndDumpHive::LoadHiveInMemory] Failed to allocate Registry Record Node!");
                delete *currentNode;
                *currentNode = NULL;

                return;
            }
        }
    }
    else
    {
        RegistryRecord* temp = new (std::nothrow) RegistryRecord;
        if (temp != NULL)
        {
            size_t pathLen = _tcslen(lpszKeyPath) + 1;
            temp->lpszPathName = new(std::nothrow) TCHAR[pathLen];
            if (temp->lpszPathName != NULL)
            {
                _tcscpy_s(temp->lpszPathName, pathLen, lpszKeyPath);

                temp->iValueCount = cValues;
                temp->iSubKeyCount = cSubKeys;

                temp->next = NULL;
                temp->prev = (*currentNode);
                temp->childNodes = NULL;
                temp->endPtr = &(*currentNode)->childNodes;

                (*currentNode)->next = temp;
            }
            else
            {
                printf("[CLoadAndDumpHive::LoadHiveInMemory] Failed to allocate Registry Record Node!");
                delete temp;
                temp = NULL;

                return;
            }
        }
    }

    for (i = 0; i < cSubKeys; i++)
    {
        cbName = MAX_KEY_LENGTH;
        retCode = RegEnumKeyEx(hCurKey, i,
            achKey,
            &cbName,
            NULL,
            NULL,
            NULL,
            NULL);

        if (retCode == ERROR_SUCCESS)
        {
            size_t nKeyLength = _tcslen(lpszSubKey) + _tcslen(_T("\\")) + _tcslen(achKey) + 1;
            TCHAR *lpszCurrentSubKey = new (std::nothrow)TCHAR[nKeyLength];

            if (lpszCurrentSubKey != NULL)
            {
                if (_tcslen(lpszSubKey) == 0)
                    _stprintf_s(lpszCurrentSubKey, nKeyLength, _T("%s"), achKey);
                else
                    _stprintf_s(lpszCurrentSubKey, nKeyLength, _T("%s\\%s"), lpszSubKey, achKey);

                //printf("\n%s", lpszCurrentSubKey);
                LoadHiveInMemory(hRootKey, lpszCurrentSubKey, (*currentNode)->endPtr);
                if (*((*currentNode)->endPtr) != NULL)
                    (*currentNode)->endPtr = &(*(*currentNode)->endPtr)->next;
                delete[] lpszCurrentSubKey;
            }
            else
            {
                printf("[CLoadAndDumpHive::LoadHiveInMemory] Error constructing registry key path!");
            }
        }

    }
    RegCloseKey(hCurKey);
    delete[] lpszKeyPath;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Serialize hive. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="pContext"> [in,out] If non-null, the context. </param>
///
/// <returns>   True if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLoadAndDumpHive::SerializeHive(CRegistryHive *pContext)
{
    if (NULL == pContext)
    {
        printf("[Error]-> Hive context missing. Serialize cannot execute.");
        return false;
    }

    printf("\n[Info]Loading => %x", (LONG)pContext->GetRootKey());
    RegistryRecord *pRecord = NULL;

    LoadHiveInMemory(pContext->GetRootKey(), _T(""), &pRecord);
    if (pRecord)
    {
        pContext->SetStartNode(pRecord);

        EnterCriticalSection(&m_cs);
        printf("\n[Info]Dumping => %x", (LONG)pContext->GetRootKey());
        DumpHiveInFile(pRecord);
        printf("\n[Info]Done => %x", (LONG)pContext->GetRootKey());
        LeaveCriticalSection(&m_cs);
    }
    return true;
}
