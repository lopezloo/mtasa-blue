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
    Try to disable:
        0x556070 CRope::CreateRopeAttacherEntity - rope will be created without default magnet/wrecking ball
        0x5569C0 CRope::AttachEntity - propably rope (RopeAttacherEntity) lose magnet ability (other solution: set vehicles bWinchCanPickMeUp to false)
        0x556030 CRope::ReleasePickedUpObject - important - player will not be able to release attached entity on all(?) ropes by pressing <somekeyheremaybectrlbydefault>

    Investigate what RopeEntity & RopeHolder is and why it seems to be ignored.

    Problems:
        1) m_pRopeAttacherEntity (=magnet) lag as fuck. SA bug which occur on all versions, even consoles.
           Magnet will go back instantly to proper position when touch some entity.
           If player press LPM the magnet go down a little for a while (sometimes?)
           Can be reduced by calling Process(All) manually.
        
*/

#include "StdInc.h"

CRopeSA::CRopeSA ( CRopeSAInterface * pRopeInterface )
{
    m_pInterface = pRopeInterface;
}

void CRopeSA::Remove ( void )
{
    g_pCore->GetConsole()->Print("CRopeSA::Remove");

    // Make sure rope still exists (currently, sometimes it disappear)
    if ( !IsRopeOwnedByCrane() )
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

// Make entity directly attached to rope (as magnet)
void CRopeSA::SetAttacherEntity ( CEntity * pRopeAttacherEntity )
{
    if ( pRopeAttacherEntity == NULL )
    {
        SetAttachedEntity ( NULL );
        g_pCore->GetConsole()->Print("SetAttacherEntity null");
        if ( m_pInterface->m_pRopeAttacherEntity->nType == 2 ) // if vehicle
        {
            // For some reason replacing vehicle with null cause crash.
            g_pCore->GetConsole()->Print("SetAttacherEntity: current attacher is vehicle, can't detach");
            // Currently vehicle is destroyed with rope by GTA

            // Calm down
            //CVehicleSA * pRopeAttacherEntityVehicleSA = (CVehicleSA *) m_pInterface->m_pRopeAttacherEntity;
            //pRopeAttacherEntityVehicleSA->SetMoveSpeed ( &CVector() );
            return;
        }
        
        m_pInterface->m_pRopeAttacherEntity = NULL;
        //m_pInterface->m_ucFlags2 = 0;
        return;
    }
    CEntitySA * pRopeAttacherEntitySA = dynamic_cast < CEntitySA* > ( pRopeAttacherEntity );
    m_pInterface->m_pRopeAttacherEntity = pRopeAttacherEntitySA->GetInterface ();

    // as in CreateRopeAttacherObject
    // bApplyGravity = 1
    // static = false

    //m_pInterface->m_ucFlags2 = 0;
}

// Attach entity to rope attacher
// Can be also via 0x5569C0 CRope::AttachEntity(CEntity *pEntityToAttach)
//        (but disabling ^ function would be great, because some rope types - or all - have magnet ability)
// Is this needed? With current attachElements function should be possible attach some element to RopeAttacherEntity. Needs investigate.
// But in that case rope will have incorrect mass? (btw. is mass settable?)
void CRopeSA::SetAttachedEntity ( CEntity * pEntityToAttach )
{
#if 0
    if ( m_pInterface->m_pRopeAttacherEntity ) // 'magnet'
    {
        CEntitySA * pEntityToAttachSA = dynamic_cast < CEntitySA* > ( pEntityToAttach );
        m_pInterface->m_pAttachedEntity = pEntityToAttachSA->GetInterface ();
    }
#endif
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
    //if ( ucSegment < m_pInterface->m_ucSegmentCount )
        m_pInterface->m_vecSegments [ ucSegment ] = vecPosition;
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

// (propably) Detach object which is attached to magnet
void CRopeSA::ReleasePickedUpObject ( )
{
    DWORD dwThis = ( DWORD ) GetInterface ();
    DWORD dwFunc = FUNC_CRope_ReleasePickedUpObject;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

// Adjusts rope start position. Something is wrong here.
// UpdateWeightInRope
void CRopeSA::Adjust ( const CVector & vecPosition )
{
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
        mov     bReturn, al;
    }
    return bReturn;
}
