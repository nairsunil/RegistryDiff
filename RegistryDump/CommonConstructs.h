#pragma once

class CRegistryHive;

struct RegistryRecord
{
    TCHAR   *lpszPathName;
    int     iValueCount;
    int     iSubKeyCount;

    RegistryRecord  **endPtr;
    RegistryRecord  *childNodes;
    RegistryRecord  *next, *prev;
    
    RegistryRecord() : lpszPathName(NULL), childNodes(NULL), 
        next(NULL), prev(NULL), endPtr(NULL)
    {
    }
};

class IHiveSerializeAlgorithm
{
public:
    virtual bool SerializeHive(CRegistryHive *) = 0;
};

class IThreadPoolTask
{
public:
    virtual bool ExecuteTask() = 0;
};