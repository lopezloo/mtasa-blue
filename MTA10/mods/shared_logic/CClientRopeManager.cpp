/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRopeManager.cpp
*  PURPOSE:     Rope entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

unsigned int CClientRopeManager::m_uiStreamedInCount = 0;

CClientRopeManager::CClientRopeManager ( CClientManager* pManager )
{
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
}


CClientRopeManager::~CClientRopeManager ( void )
{
    // Delete all the ropes
    DeleteAll ();
}

void CClientRopeManager::DoPulse ( void )
{
    UpdateLimitInfo ();

    // Force ropes to update
    g_pGame->GetRopes ()->Update ();

    // Loop through all our streamed-in ropes
    for ( uint i = 0 ; i < m_StreamedIn.size () ; i++ )
    {
        CClientRope* pRope = m_StreamedIn[i];

        assert ( pRope->GetGameRope () );
        pRope->StreamedInPulse ();
    }
}


void CClientRopeManager::DeleteAll ( void )
{
    // Delete all the ropes
    m_bCanRemoveFromList = false;
    std::vector < CClientRope* > ::const_iterator iter = m_Ropes.begin ();
    for ( ; iter != m_Ropes.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_Ropes.clear ();
    m_bCanRemoveFromList = true;
}


CClientRope* CClientRopeManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTROPE )
    {
        return static_cast < CClientRope* > ( pEntity );
    }

    return NULL;
}

bool CClientRopeManager::IsRopeLimitReached ( void )
{
    return ( m_uiStreamedInCount >= MAX_ROPES );
}

void CClientRopeManager::RemoveFromLists ( CClientRope * pRope )
{
    if ( m_bCanRemoveFromList )
    {
        ListRemove ( m_Ropes, pRope );
    }
    ListRemove ( m_StreamedIn, pRope );
}

void CClientRopeManager::OnCreation ( CClientRope * pRope )
{
    m_StreamedIn.push_back ( pRope );
    UpdateLimitInfo ();
}


void CClientRopeManager::OnDestruction ( CClientRope * pRope )
{
    ListRemove ( m_StreamedIn, pRope );
    UpdateLimitInfo ();
}

void CClientRopeManager::UpdateLimitInfo ( void )
{
    // remove this later
}
