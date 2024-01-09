// RegistryDiffDump_C++.cpp : Defines the entry point for the console application.
//

#include "..\\stdafx.h"
#include "HiveSerializationManager.h"
#include <conio.h>
#include <time.h>

int _tmain(int argc, _TCHAR* argv[])
{
    clock_t tStart;
    tStart = clock();
    if (CHiveSerializationMgr::instance().Initialize())
    {
        CHiveSerializationMgr::instance().SetHiveForSerialization(HKEY_CLASSES_ROOT);
        CHiveSerializationMgr::instance().SetHiveForSerialization(HKEY_CURRENT_USER);
        CHiveSerializationMgr::instance().SetHiveForSerialization(HKEY_LOCAL_MACHINE);
        CHiveSerializationMgr::instance().SetHiveForSerialization(HKEY_USERS);
        CHiveSerializationMgr::instance().SetHiveForSerialization(HKEY_CURRENT_CONFIG);

        CHiveSerializationMgr::instance().SerializeAll();
    }
   
    CHiveSerializationMgr::instance().WaitForCompletion();

    printf("\n[Info] Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);

    return 0;
}

