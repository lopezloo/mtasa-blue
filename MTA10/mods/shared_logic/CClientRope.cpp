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

CClientRope::CClientRope ( class CClientManager* pManager, ElementID ID, CVector vecPosition, CClientEntity * pRopeHolder )
    : ClassInit ( this )
    , CClientStreamElement ( pManager->GetRopeStreamer (), ID )
{
    m_pManager = pManager;
    m_pRopeManager = pManager->GetRopeManager ();

    m_pHolderEntity = pRopeHolder;

    m_pRope = NULL;
    m_pAttacherEntity = NULL;
    m_pAttachedEntity = NULL;
  //m_fMass = 20.0f;
    m_fSegmentLength = 0.2f;
  //m_bSegmentGroundCheck = true;

    // m_ucSegmentCount + 1 segments become dynamic
    // Segments between 0 <-> m_ucSegmentCount are static line.

    // Init all segments
    for ( int i = 0; i < MAX_ROPE_SEGMENTS; i++ )
    {
        m_vecSegmentsPosition [i] = CVector ( vecPosition.fX, vecPosition.fY, vecPosition.fZ - i*m_fSegmentLength );
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
            m_pRope = g_pGame->GetRopes ()->CreateRope ( m_vecSegmentsPosition[0], m_pHolderEntity->GetGameEntity (), 0 );

            if ( m_pRope )
            {
                // Update segment data
                for ( int i = 0; i < MAX_ROPE_SEGMENTS; i++ )
                {
                    m_pRope->SetSegmentPosition ( i, m_vecSegmentsPosition[i] );
                }
                m_pRope->SetSegmentLength ( m_fSegmentLength );

                if ( m_pAttacherEntity )
                {
                    m_pRope->SetAttacherEntity ( m_pAttacherEntity->GetGameEntity () );
                }

                if ( m_pAttachedEntity )
                {
                    m_pRope->SetAttachedEntity ( m_pAttachedEntity->GetGameEntity () );
                }

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
    //m_pRope->Update ();
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
    // Save segment positions
    for ( int i = 0; i < MAX_ROPE_SEGMENTS; i++ )
    {
        m_vecSegmentsPosition [i] = m_pRope->GetSegmentPosition ( i );
    }

    // Destroy the rope
    Destroy ( );
}

// Minor stuff \/
bool CClientRope::SetSegmentPosition ( uchar ucSegment, CVector vecPosition )
{
    if ( ucSegment < MAX_ROPE_SEGMENTS )
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
        return true;
    }
    return false;
}

bool CClientRope::GetSegmentPosition ( uchar ucSegment, CVector & vecPosition )
{
    if ( ucSegment < MAX_ROPE_SEGMENTS )
    {
        if ( m_pRope )
        {
            m_vecSegmentsPosition [ ucSegment ] = m_pRope->GetSegmentPosition ( ucSegment );
        }
        vecPosition = m_vecSegmentsPosition [ ucSegment ];
        return true;
    }
    return false;
}

void CClientRope::SetPosition ( const CVector & vecPosition )
{
    if ( m_pRope )
    {
        m_pRope->SetSegmentPosition ( 0, ( CVector ) vecPosition );
    }

    m_vecSegmentsPosition [ 0 ] = vecPosition;
    UpdateStreamPosition ( vecPosition );
}

void CClientRope::SetSegmentLength ( float fSegmentLength )
{
    if ( fSegmentLength > 0 )
    {
        if ( m_pRope )
        {
            m_pRope->SetSegmentLength ( fSegmentLength );
        }

        m_fSegmentLength = fSegmentLength;
    }
}

float CClientRope::GetSegmentLength ( void )
{
    return m_fSegmentLength;
}

// Attaching
// Try to handle it later as attachElements.
void CClientRope::SetAttacherEntity ( CClientEntity * pAttacherEntity )
{
    m_pAttacherEntity = pAttacherEntity;
    if ( m_pRope )
    {
        m_pRope->SetAttacherEntity ( pAttacherEntity ? pAttacherEntity->GetGameEntity () : NULL );
    }

    // Disable object break ability. This cause only weird effect so user can re-enable it later.
    if ( pAttacherEntity && pAttacherEntity->GetType () == ENTITY_TYPE_OBJECT )
    {
        CClientObject * pAttacherObject = ( CClientObject * ) pAttacherEntity;
        pAttacherObject->SetBreakable ( false );
    }

}

bool CClientRope::SetAttachedEntity ( CClientEntity * pEntityToAttach )
{
    if ( m_pAttacherEntity == NULL )
        return false;

    m_pAttachedEntity = pEntityToAttach;
    if ( m_pRope )
    {
        m_pRope->SetAttachedEntity ( pEntityToAttach ? pEntityToAttach->GetGameEntity () : NULL );
    }
    return true;
}

void CClientRope::SetHolderEntity ( CClientEntity * pHolderEntity )
{
    m_pHolderEntity = pHolderEntity;
    if ( m_pRope )
    {
        m_pRope->SetHolderEntity ( pHolderEntity->GetGameEntity () );
    }
}