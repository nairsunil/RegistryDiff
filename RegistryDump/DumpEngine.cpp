#include "DumpEngine.h"
#include <exception>
#include <stdio.h>

CDumpEngine::CDumpEngine()
{
    m_isInitialized = false;
}


CDumpEngine::~CDumpEngine()
{

}


bool CDumpEngine::Initialize(){

    //Initialize the ThreadPool Manager and keep it ready for execution
    try
    {
        m_thPoolMgr.Initalize();
        m_isInitialized = true;
    }
    catch (std::exception &ex)
    {
        printf("[CDumpEngine::Initialize] -> %s", ex.what());
        m_isInitialized = false;

        return false;
    }

    return true;
}

