/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRope.cpp
*  PURPOSE:     Rope entity class
*
*****************************************************************************/

#include <StdInc.h>
#include "CClientRope.h"

//m_pManager, pRopeEntity, vecPosition, ucSegments, pRopeHolder
//CClientRope::CClientRope ( CClientManager * pManager, ElementID ID, CClientEntity * pRopeEntity, CVector vecPosition, uchar ucSegments, CClientEntity * pRopeHolder ) : ClassInit ( this ), CClientStreamElement ( pManager->GetRopeStreamer (), ID )
//CClientRope::CClientRope ( class CClientManager* pManager, ElementID ID, CClientEntity * pRopeEntity, CVector vecPosition, uchar ucSegments, CClientEntity * pRopeHolder ) : ClassInit ( this ), CClientEntity ( ID )

CClientRope::CClientRope ( class CClientManager* pManager, ElementID ID, CClientEntity * pRopeEntity, CVector vecPosition, uchar ucSegments, CClientEntity * pRopeHolder )
    : ClassInit ( this )
    , CClientStreamElement ( pManager->GetRopeStreamer (), ID )
{
    m_pManager = pManager;
    m_pRopeManager = pManager->GetRopeManager ();

    m_pRopeEntity = pRopeEntity;
    m_pRopeHolder = pRopeHolder;

    m_pRope = NULL;
    m_pRopeAttacherEntity = NULL;
    m_pAttachedEntity = NULL;
    //m_fMass = -1.0f; // is this settable? maybe it's attached entities (attacher + attached?) mass or something

    // maybe do it in constructor later
    m_ucSegmentCount = ucSegments;
    m_fSegmentLength = 2.5f;
    m_bStreamedIn = true;

    m_vecSegmentsPosition [0] = vecPosition;
    for ( int i = 1; i < m_ucSegmentCount; i++ )
    {
        m_vecSegmentsPosition [i] = CVector ( ); // ?
    }

    SetTypeName ( "rope" );

    m_pRopeManager->AddToList ( this );
}

CClientRope::~CClientRope ( void )
{
    // Detach us from anything
    //AttachTo ( NULL );

    // Destroy the rope
    Destroy ();

    // Remove us from the list
    Unlink ();
}

void CClientRope::Unlink ( void )
{
    m_pRopeManager->RemoveFromLists ( this );
}

void CClientRope::Create ( void )
{
    // Not already created an rope?
    if ( !m_pRope )
    {
        if ( !CClientRopeManager::IsRopeLimitReached () )
        {
            // Create the rope
            m_pRope = g_pGame->GetRopes ()->CreateRope ( m_pRopeEntity->GetGameEntity (), m_vecSegmentsPosition[0], m_ucSegmentCount, m_pRopeHolder->GetGameEntity () );

            if ( m_pRope )
            {
                // Apply our data to the rope
                // bla bla

                //UpdateVisibility ();

                // Reattach to an entity + any entities attached to this
                //ReattachEntities ();

                // Validate this entity again
                m_pManager->RestoreEntity ( this );

                // Tell the streamer we've created this object
                NotifyCreate ();

                ++m_pRopeManager->m_uiStreamedInCount;

                // Done
                return;
            }
        }

        // Tell the streamer we could not create it
        NotifyUnableToCreate ();
    }
}

void CClientRope::Destroy ( void )
{
    // If the rope exists
    if ( m_pRope )
    {
        // Invalidate
        m_pManager->InvalidateEntity ( this );

        // Detach attacher
        //m_pRope->SetAttacherEntity ( NULL );

        // Destroy the rope
        m_pRope->Remove ( );
        m_pRope = NULL;

        NotifyDestroy ();

        --m_pRopeManager->m_uiStreamedInCount;
    }
}

void CClientRope::StreamedInPulse ( void )
{
    //g_pCore->GetConsole()->Print("CClientRope::StreamedInPulse");
    // blah blah, maybe update segments here?
}

void CClientRope::NotifyCreate ( void )
{
    m_pRopeManager->OnCreation ( this );
    CClientStreamElement::NotifyCreate ();
}


void CClientRope::NotifyDestroy ( void )
{
    m_pRopeManager->OnDestruction ( this );
}

void CClientRope::StreamIn ( bool bInstantly )
{
    Create ( );
}

void CClientRope::StreamOut ( void )
{
    // Maybe save some data
    // ?

    // Destroy the rope
    Destroy ();
}

// Minor stuff \/
void CClientRope::SetSegmentPosition ( uchar ucSegment, CVector vecPosition )
{
    if ( ucSegment < m_ucSegmentCount )
    {
        if ( m_pRope )
        {
            m_pRope->SetSegmentPosition ( ucSegment, vecPosition );
        }

        m_vecSegmentsPosition [ ucSegment ] = vecPosition;

        if ( ucSegment == 0 )
        {
            // First segment so update stream position
            UpdateStreamPosition ( vecPosition );
        }
    }
}

CVector CClientRope::GetSegmentPosition ( uchar ucSegment )
{
    if ( ucSegment < m_ucSegmentCount )
    {
        if ( m_pRope )
        {
            return m_pRope->GetSegmentPosition ( ucSegment );
        }
        return m_vecSegmentsPosition [ ucSegment ];
    }
    return CVector ();
}

void CClientRope::SetPosition ( const CVector& vecPosition )
{
    if ( m_pRope )
    {
        //m_pRope->SetSegmentPosition ( 0, (CVector) vecPosition );
        m_pRope->Adjust ( vecPosition );
    }

    m_vecSegmentsPosition [ 0 ] = vecPosition; // ?
    UpdateStreamPosition ( vecPosition );
}

void CClientRope::SetSegmentCount ( uchar ucSegmentCount )
{
    if ( ucSegmentCount < MAX_ROPE_SEGMENTS )
    {
        if ( m_pRope )
        {
            m_pRope->SetSegmentCount ( ucSegmentCount );
        }

        m_ucSegmentCount = ucSegmentCount;
    }
}

uchar CClientRope::GetSegmentCount ( void )
{
    return m_ucSegmentCount;
}

void CClientRope::SetSegmentLength ( float fSegmentLength )
{
    if ( m_pRope )
    {
        m_pRope->SetSegmentLength ( fSegmentLength );
    }

    m_fSegmentLength = fSegmentLength;
}

float CClientRope::GetSegmentLength ( void )
{
    return m_fSegmentLength;
}

// Attaching
void CClientRope::SetAttacherEntity ( CClientEntity * pAttacherEntity )
{
    if ( m_pRope )
    {
        if ( pAttacherEntity == NULL )
        {
            m_pRope->SetAttacherEntity ( NULL );
        }
        else
        {
            m_pRope->SetAttacherEntity ( pAttacherEntity->GetGameEntity () );
        }
    }
    m_pRopeAttacherEntity = pAttacherEntity;
}

void CClientRope::SetAttachedEntity ( CClientEntity * pEntityToAttach )
{
    if ( m_pRope )
    {
        m_pRope->SetAttachedEntity ( pEntityToAttach->GetGameEntity () );
    }
    m_pAttachedEntity = pEntityToAttach;
}

bool CClientRope::DetachElementFromRope ( CClientEntity * pAttachedElement )
{
    if ( pAttachedElement == m_pAttachedEntity )
    {
        //SetAttachedEntity ( NULL );
        return true;
    }
    else if ( pAttachedElement == m_pRopeAttacherEntity )
    {
        SetAttacherEntity ( NULL );
        return true;
    }
    return false;
}
