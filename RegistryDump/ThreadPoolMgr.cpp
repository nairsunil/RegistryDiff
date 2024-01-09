////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	ThreadPoolMgr.cpp
//
// summary:	Implements the thread pool manager class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ThreadPoolMgr.h"
#include <exception>
#include <Windows.h>

volatile LONG CThreadPoolMgr::nTaskCount = 0;
HANDLE CThreadPoolMgr::sHTaskComplete;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Gets current module. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <returns>   The current module. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

HANDLE GetCurrentModule()
{
    DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS;
    HMODULE hm = nullptr;
    GetModuleHandleEx(flags, reinterpret_cast<LPCTSTR>(GetCurrentModule), &hm);
    return hm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Callbacks. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="instance"> The instance. </param>
/// <param name="context">  The context. </param>
/// <param name="work">     The work. </param>
///
/// <returns>   A CALLBACK. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CALLBACK Callback(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
    InterlockedIncrement(&CThreadPoolMgr::nTaskCount);

    CloseThreadpoolWork(work);
    IThreadPoolTask *pTask = reinterpret_cast<IThreadPoolTask *>(context);
    if (pTask)
        pTask->ExecuteTask();

    if (InterlockedDecrement(&CThreadPoolMgr::nTaskCount) == 0)
        SetEvent(CThreadPoolMgr::sHTaskComplete);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Default constructor. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CThreadPoolMgr::CThreadPoolMgr()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Destructor. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CThreadPoolMgr::~CThreadPoolMgr()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Initalizes this object. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <returns>   True if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

bool CThreadPoolMgr::Initalize()
{
    m_pool = CreateThreadpool(NULL);
    if (NULL == m_pool)
    {
        printf("[CThreadPoolMgr::Initalize] Failed to intialize the ThreadPool.");
        return false;
    }

    // 2. Initialize the Thread-Pool Environment struct:
    InitializeThreadpoolEnvironment(&m_environment);
    SetThreadpoolCallbackPool(&m_environment, m_pool);
    SetThreadpoolCallbackLibrary(&m_environment, GetCurrentModule());

    // 3. Create Thread-Pool Clean-up Group:
    m_cleanupGroup = CreateThreadpoolCleanupGroup();
    if (NULL == m_cleanupGroup)
    {
        DestroyThreadpoolEnvironment(&m_environment);
        CloseThreadpool(m_pool);
        printf("[CThreadPoolMgr::Initalize] Could not create a thread pool clean-up group!");
        return false;
    }

    SetThreadpoolCallbackCleanupGroup(&m_environment, m_cleanupGroup, NULL);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Adds a task. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="pTask">    [in,out] If non-null, the task. </param>
///
/// <returns>   A PTP_WORK. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

PTP_WORK CThreadPoolMgr::AddTask(IThreadPoolTask *pTask)
{
    PTP_WORK work = CreateThreadpoolWork(Callback, pTask, &m_environment);
    if (work == NULL)
    {
        return NULL;
    }

    SubmitThreadpoolWork(work);
    return work;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Cleanups this object. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CThreadPoolMgr::Cleanup()
{
    if (m_pool != NULL)
    {
        CloseThreadpoolCleanupGroupMembers(m_cleanupGroup, false, NULL);
        CloseThreadpoolCleanupGroup(m_cleanupGroup);
        DestroyThreadpoolEnvironment(&m_environment);
        CloseThreadpool(m_pool);
        m_pool = NULL;
    }
}

