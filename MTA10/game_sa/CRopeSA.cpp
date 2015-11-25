/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRopeSA.cpp
*  PURPOSE:     Rope entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               JoeBullet <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

/*
    v7549
    Try to disable:
        0x556070 CRopes::CreateHookObjectForRope - rope will be created without default magnet/wrecking ball
        0x5569C0 CRope::PickUpObject - propably rope (RopeAttacherEntity) lose magnet ability (other solution: set vehicles bWinchCanPickMeUp to false)
        0x556030 CRope::ReleasePickedUpObject - important - player will not be able to release attached entity on all(?) ropes by pressing <somekeyheremaybectrlbydefault> [done]

    Investigate what RopeEntity & RopeHolder is and why it seems to be ignored.

    * Entities needs to be re-attached after model change.
    * Crash in CRopeSA::Remove if attacher (vehicle, train) was destroyed before.

    Problems:
        1) m_pRopeAttacherEntity (=magnet) lag as fuck. SA bug which occur on all versions, even consoles.
           Magnet will go back instantly to proper position when touch some entity.
           If player press LPM the magnet go down a little for a while (sometimes?)
           Can be reduced by calling CRopes::Update manually.

           Can't reproduce this now. Maybe it occurs only if rope was created initialy as NON-SWAT (with magnet).
        
*/

#include "StdInc.h"

CRopeSA::CRopeSA ( CRopeSAInterface * pRopeInterface )
{
    m_pInterface = pRopeInterface;
}

void CRopeSA::Remove ( void )
{
    g_pCore->GetConsole()->Print("CRopeSA::Remove");

    // Make sure rope still exists (currently, sometimes it disappear). This check don't work for swat ropes (8)
    if ( !IsRopeOwnedByCrane() && m_pInterface->m_ucRopeType < ROPE_SWAT )
    {
        g_pCore->GetConsole()->Print("something was fucked up");
        return;
    }

    // Detach our custom magnet first (if we don't do this magnet will be destroyed)
    SetAttacherEntity ( NULL );

    DWORD dwFunc = FUNC_CRope_Destructor;
    DWORD dwThis = ( DWORD ) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

// Set rope holder
void CRopeSA::SetHolderEntity ( CEntity * pHolderEntity )
{
    GetInterface ()->m_pRopeHolder = ( CEntitySAInterface * ) pHolderEntity;
    GetInterface ()->m_pRopeEntity = ( CEntitySAInterface * ) pHolderEntity->GetInterface() + 29; // ?
}

// Make entity directly attached to rope (like magnet)
void CRopeSA::SetAttacherEntity ( CEntity * pRopeAttacherEntity )
{
    // Vehicle replaced with object is still attached to rope but velocity bug seems to be fixed.
    if ( pRopeAttacherEntity == NULL )
    {
        g_pCore->GetConsole()->Print("SetAttacherEntity null");
        if ( !m_pInterface->m_pRopeAttacherEntity )
            return;

        SetAttachedEntity ( NULL );
        if ( m_pInterface->m_pRopeAttacherEntity->nType == ENTITY_TYPE_VEHICLE )
        {
            // For some reason replacing vehicle with null cause crash. Currently vehicle is destroyed with rope by GTA
            g_pCore->GetConsole()->Print("SetAttacherEntity: current attacher is vehicle, can't detach");
            return;
        }
        
        m_pInterface->m_pRopeAttacherEntity = NULL;
        m_pInterface->m_ucRopeType = ROPE_SWAT;
        return;
    }

    // Make sure entity is dynamic
    if ( pRopeAttacherEntity->GetEntityType ( ) == ENTITY_TYPE_OBJECT )
    {
        // Problem: MTA don't know default entity physical data so we can't currently reset it while detach. Simplest way to fix would be recreate.
        /*CPhysicalSAInterface * pRopeAttacherPhysicalSAInterface = ( CPhysicalSAInterface * ) pRopeAttacherEntity->GetInterface ();
        pRopeAttacherPhysicalSAInterface->bDisableMovement = false;
        pRopeAttacherPhysicalSAInterface->bDisableFriction = false;
        pRopeAttacherPhysicalSAInterface->bApplyGravity = true;*/

        CPhysicalSA * pRopeAttacherPhysicalSA = dynamic_cast < CPhysicalSA* > ( pRopeAttacherEntity );
        pRopeAttacherPhysicalSA->AddToMovingList ();
        pRopeAttacherPhysicalSA->SetStatic ( false );
        pRopeAttacherPhysicalSA->SetMoveSpeed ( &CVector(0, 0, 0.1) );
        g_pCore->GetConsole()->Print("CRopeSA::SetAttacherEntity object blah blah");
    }

    m_pInterface->m_pRopeAttacherEntity = pRopeAttacherEntity->GetInterface ( );
    m_pInterface->m_ucRopeType = ROPE_INDUSTRIAL;

    /*
        Is this used by ropes? Test me.
        CEntitySAInterface * m_pAttachedEntity;   // 252
        CVector m_vecAttachedOffset;    // 256
        CVector m_vecAttachedRotation;    // 268
    */
}

// Attach entity to rope attacher (magnet)
// Can be also via 0x5569C0 CRope::AttachEntity(CEntity *pEntityToAttach)
//        (but disabling ^ function would be great, because some rope types - or all - have magnet ability)
// Is this needed? With current attachElements function should be possible attach some element to RopeAttacherEntity. Needs investigate.
// But in that case rope will have incorrect mass? (btw. is mass settable?)
/*void CRopeSA::SetAttachedEntity ( CEntity * pEntityToAttach )
{
    if ( m_pInterface->m_pRopeAttacherEntity ) // attacher must exist
    {
        if ( pEntityToAttach == NULL )
        {
            m_pInterface->m_pAttachedEntity = NULL;
            return;
        }

        CEntitySA * pEntityToAttachSA = dynamic_cast < CEntitySA* > ( pEntityToAttach );

        // Problem: MTA don't know default entity physical data so we can't currently reset it while detach.
        CPhysicalSAInterface * pEntityToAttachPhysicalSAInterface = ( CPhysicalSAInterface * ) pEntityToAttachSA->GetInterface ();
        // We need also https://github.com/multitheftauto/mtasa-blue/pull/12 goddamit

        // Make sure entity is dynamic
        if ( pEntityToAttachSA->GetInterface ()->nType == ENTITY_TYPE_OBJECT )
        {
            pEntityToAttachPhysicalSAInterface->bDisableMovement = false;
            pEntityToAttachPhysicalSAInterface->bDisableFriction = false;
            pEntityToAttachPhysicalSAInterface->bApplyGravity = true;

            CPhysicalSA * pEntityToAttachPhysicalSA = dynamic_cast < CPhysicalSA* > ( pEntityToAttach );
            pEntityToAttachPhysicalSA->AddToMovingList ();
            pEntityToAttachPhysicalSA->SetStatic ( false );
            pEntityToAttachPhysicalSA->SetMoveSpeed ( &CVector(0, 0, 0.1) );
            g_pCore->GetConsole()->Print("CRopeSA::SetAttachedEntity object blah blah");
        }

        m_pInterface->m_pAttachedEntity = pEntityToAttachSA->GetInterface ();
    }
}*/

// Version 2.
void CRopeSA::SetAttachedEntity ( CEntity * pEntityToAttach )
{
    if ( m_pInterface->m_pRopeAttacherEntity )
    {
        DWORD dwThis = ( DWORD ) m_pInterface;
        if ( pEntityToAttach == NULL )
        {
            DWORD dwFunc = FUNC_CRope_ReleasePickedUpObject;
            _asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }
        }
        else
        {
            if ( pEntityToAttach->GetEntityType() == ENTITY_TYPE_OBJECT )
            {
                pEntityToAttach->SetStatic ( false );
            }

            DWORD dwEntityToAttachSAInterface = ( DWORD ) pEntityToAttach->GetInterface ();
            DWORD dwFunc = FUNC_CRope_PickUpObject;
            _asm
            {
                mov     ecx, dwThis
                push    dwEntityToAttachSAInterface
                call    dwFunc
                add     esp, 4
            }
        }
        // Force attacher to update
        //SetAttacherEntity ( (CEntity *) m_pInterface->m_pRopeAttacherEntity );
    }
}

uchar CRopeSA::GetSegmentCount ( void )
{
    return m_pInterface->m_ucSegmentCount;
}

void CRopeSA::SetSegmentCount ( uchar ucSegmentCount )
{
    if ( ucSegmentCount < MAX_ROPE_SEGMENTS )
    {
        m_pInterface->m_ucSegmentCount = ucSegmentCount;
    }
}

CVector CRopeSA::GetSegmentPosition ( uchar ucSegment )
{
    //if ( ucSegment < m_pInterface->m_ucSegmentCount )
        return m_pInterface->m_vecSegments [ ucSegment ];
    //return CVector (); // ?
}

// Set segment position
void CRopeSA::SetSegmentPosition ( uchar ucSegment, CVector & vecPosition )
{
    if ( ucSegment < MAX_ROPE_SEGMENTS )
    {
        m_pInterface->m_vecSegments [ ucSegment ] = vecPosition;
    }
}

// Get/set all segments length
float CRopeSA::GetSegmentLength ( void )
{
    return m_pInterface->m_fSegmentLength;
}

void CRopeSA::SetSegmentLength ( float fLength )
{
    m_pInterface->m_fSegmentLength = fLength;
}

// Adjusts rope start position. Something is wrong here - use SetSegmentPosition instead.
// UpdateWeightInRope
void CRopeSA::Adjust ( const CVector & vecPosition )
{
#if 0
    g_pCore->GetConsole()->Print("CRopeSA::Adjust");

    DWORD dwThis = ( DWORD ) GetInterface ();
    DWORD dwFunc = FUNC_CRope_Adjust;

    float fX = vecPosition.fX;
    float fY = vecPosition.fY;
    float fZ = vecPosition.fZ;

    float fUnknown = 1.0f;
    //int iUnknown = 1;
    //CVector pOutVec = CVector(0, 0, 0);
    CVector pOutVec = vecPosition;
    //CVector * pOutVec = &CVector ();

    // UpdateWeightInRope ( float fX, float fY, float fZ, float fUnknown, CVector * pOutVec ) 
    _asm
    {
        mov     ecx, dwThis
        push    pOutVec
        push    fUnknown
        push    fZ
        push    fY
        push    fX
        call    dwFunc
        //add     esp, 4*5
    }

    // UpdateWeightInRope ( CVector vecPosition, float fUnknown, CVector * pOutVec ) 
    /*_asm
    {
        mov     ecx, dwThis
        push    pOutVec
        push    fUnknown
        push    vecPosition
        call    dwFunc
        //add     esp, 4*3
    }*/
#endif
}

bool CRopeSA::IsRopeOwnedByCrane ( )
{
    DWORD dwThis = ( DWORD ) GetInterface ();
    DWORD dwFunc = FUNC_CRope_IsRopeOwnedByCrane;
    bool bReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}
