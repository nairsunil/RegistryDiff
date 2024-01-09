////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	RegistryHive.cpp
//
// summary:	Implements the registry hive class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "..\\stdafx.h"
#include "CommonConstructs.h"
#include "RegistryHive.h"
#include <exception>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Constructor. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="hRootKey"> Handle of the root key. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

CRegistryHive::CRegistryHive(HKEY hRootKey)
{
    m_hRootKey = hRootKey;
    m_pRegistryStartNode = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Destructor. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

CRegistryHive::~CRegistryHive()
{
    if (m_pSerializeAlgo)
    {
        delete m_pSerializeAlgo;
        m_pSerializeAlgo = NULL;
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Executes the task operation. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <returns>   True if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

bool CRegistryHive::ExecuteTask()
{
    return SerializeHive();

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Sets serialize algorithm. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <param name="pSerializeAlgo">   [in,out] If non-null, the serialize algo. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void CRegistryHive::SetSerializeAlgorithm(IHiveSerializeAlgorithm *pSerializeAlgo)
{
    m_pSerializeAlgo = pSerializeAlgo;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   Determines if we can serialize hive. </summary>
///
/// <remarks>   Nair Sn, 5/26/2017. </remarks>
///
/// <exception cref="std::exception">   Thrown when an exception error condition occurs. </exception>
///
/// <returns>   True if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

bool CRegistryHive::SerializeHive()
{
    if (NULL == m_pSerializeAlgo)
    {
        throw std::exception("Error: Serialization Algorithm is not set for hive.");
    }

    return m_pSerializeAlgo->SerializeHive(this);
}
