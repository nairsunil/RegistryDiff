////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	HiveSerializationManager.cpp
//
// summary:	Implements the hive serialization manager class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "HiveSerializationManager.h"
#include "LoadAndDumpHive.h"

CHiveSerializationMgr CHiveSerializationMgr::m_sInstance;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Default constructor. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CHiveSerializationMgr::CHiveSerializationMgr() : 
m_pTPExecutor(NULL), m_fileSerializer(NULL)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Destructor. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CHiveSerializationMgr::~CHiveSerializationMgr()
{
    m_pTPExecutor->Cleanup();
    CloseHandle(CThreadPoolMgr::sHTaskComplete);

    if (m_fileSerializer){
        delete m_fileSerializer;
        m_fileSerializer = NULL;
    }

    list<CRegistryHive*>::iterator it = m_HiveList.begin();
    while (it != m_HiveList.end())
    {
        CRegistryHive *p = *it;
        it++;

        delete p;
    }

    if (!m_HiveList.empty())
        m_HiveList.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Initializes this object. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <returns>   True if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

bool CHiveSerializationMgr::Initialize()
{
    m_bIsInitialized = false;

    CThreadPoolMgr::sHTaskComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_pTPExecutor = new(std::nothrow) CThreadPoolMgr();
    if (m_pTPExecutor)
    {
        if (m_pTPExecutor->Initalize())
        {
            m_fileSerializer = new(std::nothrow) CHiveFileSerializer();
            if (m_fileSerializer)
            {
                if (m_fileSerializer->InitSerialzier())
                    m_bIsInitialized = true;
            }
            else
            {
                printf("\n[CHiveSerializationMgr::Initialize] Failed to the allocate memort to the thread pool serializer.");
            }
        }
    }
    else
    {
        printf("\n[CHiveSerializationMgr::Initialize] Failed to allocate memory to the thread pool initializer.");
    }
    return m_bIsInitialized;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Sets hive for serialization. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="hRootKey"> Handle of the root key. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CHiveSerializationMgr::SetHiveForSerialization(HKEY hRootKey)
{
    if (!m_bIsInitialized)
    {
        printf("\n[CHiveSerializationMgr::SetHiveForSerialization] Serialization Manager not initialized. \
               Cannot proceed with the hive initilization request");
        return;
    }

    CRegistryHive *pNewHive = new CRegistryHive(hRootKey);
    InitializeCriticalSection(&CLoadAndDumpHive::m_cs);

    IHiveSerializeAlgorithm *pAlgo = new (std::nothrow) CLoadAndDumpHive(m_fileSerializer);
    if (pAlgo != NULL)
    {
        pNewHive->SetSerializeAlgorithm(pAlgo);
        m_HiveList.push_back(pNewHive);
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Serialize all. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CHiveSerializationMgr::SerializeAll()
{
    if (!m_bIsInitialized)
    {
        printf("\n[CHiveSerializationMgr::SerializeAll] Serialization Manager not initialized. \
                              Cannot proceed with the Serialize request.");
        return;
    }
    list<CRegistryHive *>::iterator itr = m_HiveList.begin();
    while (itr != m_HiveList.end())
    {
        //itr->SerializeHive();
        m_pTPExecutor->AddTask(*itr);
        itr++;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Wait for completion. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CHiveSerializationMgr::WaitForCompletion()
{
    if (!m_bIsInitialized)
    {
        printf("\n[CHiveSerializationMgr::SerializeAll] Serialization Manager not initialized. \
                                             Cannot proceed with the Wait request.");
        return;
        return;
    }
    if (!m_HiveList.empty())
    {
        printf("\nWaiting for completion....");
        WaitForSingleObject(CThreadPoolMgr::sHTaskComplete, INFINITE);
    }
}
