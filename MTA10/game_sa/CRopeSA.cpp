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
    v7569
    Try to disable:
        0x556070 CRopes::CreateHookObjectForRope - rope will be created without default magnet/wrecking ball
        0x5569C0 CRope::PickUpObject - propably rope (RopeAttacherEntity) lose magnet ability (other solution: set vehicles bWinchCanPickMeUp to false)
        0x556030 CRope::ReleasePickedUpObject - important - player will not be able to release attached entity on all(?) ropes by pressing <somekeyheremaybectrlbydefault> [done]

    Investigate what RopeEntity & RopeHolder is and why it seems to be ignored.

    * Entities needs to be re-attached after model change.
    * Crash in CRopeSA::Remove if attacher (vehicle, train) was destroyed before.
    * Crash when holder is localplayer and re-spawn.

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

    // Make sure rope still exists.
    if ( m_pInterface->m_ucRopeType == NULL )
    {
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
    m_pInterface->m_ucRopeType = ROPE_INDUSTRIAL2;

    /*
        Is this used by ropes? Test me.
        CEntitySAInterface * m_pAttachedEntity;   // 252
        CVector m_vecAttachedOffset;    // 256
        CVector m_vecAttachedRotation;    // 268
    */
}

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
            /*if ( pEntityToAttach->GetEntityType() == ENTITY_TYPE_OBJECT )
            {
                pEntityToAttach->SetStatic ( false );
            }*/

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

// Not actually segment count; todo.
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
    if ( ucSegment < m_pInterface->m_ucSegmentCount )
        return m_pInterface->m_vecSegments [ ucSegment ];
    return CVector ();
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