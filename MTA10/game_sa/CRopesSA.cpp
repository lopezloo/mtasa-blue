/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRopesSA.cpp
*  PURPOSE:     Rope manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*                JoeBullet <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
DWORD dwDurationAddress = 0x558D1El;

int CRopesSA::CreateRopeForSwatPed ( const CVector & vecPosition, DWORD dwDuration )
{
    int iReturn;
    DWORD dwFunc = FUNC_CRope_CreateRopeForSwatPed;
    CVector * pvecPosition = const_cast < CVector * > ( &vecPosition );
    // First Push @ 0x558D1D is the duration.
    MemPut((void*)(dwDurationAddress), dwDuration);
    _asm
    {
        push    pvecPosition
        call    dwFunc
        add     esp, 0x4
        mov     iReturn, eax
    }
    // Set it back for SA in case we ever do some other implementation.
    MemPut((DWORD*)(dwDurationAddress), 4000);
    return iReturn;
}

CRopesSA::CRopesSA ( )
{
    for ( int i = 0; i < MAX_ROPES; i++ )
        Ropes [i] = new CRopeSA ( ( CRopeSAInterface * ) ( ARRAY_CRopes + i * sizeof(CRopeSAInterface) ) );
}

CRopesSA::~CRopesSA ( void )
{
    for ( int i = 0; i < MAX_ROPES; i++ )
    {
        delete Ropes [i];
    }
}

/*
    Leviathan winch:
    Vehicle = 1a847218
    m_vecSegments[0] = 80000000, 4055c577, a0000000
    m_vecSegmentsReleased[0] = 0, 0, 0
    m_pRopeEntity = 1a847235 // ?
    m_pad4 = 0
    m_fMass = 0 // always 0, even something attached
    m_fSegmentLength = 40000000 // ?
    m_pRopeHolder = 1a847218 // leviathan
    m_pRopeAttacherEntity = 1a88fad0 // magnet
    m_pAttachedEntity = 0 // attached entity
    m_uiHoldEntityExpireTime = 0
    m_ucSegmentCount = 15 // changes when pressing NUM8/2
    m_ucRopeType = 1
    m_ucFlags1 = 0
    m_ucFlags2 = 4 // always 4

    m_pRopeEntity m_nModelIndex = 200
    m_pRopeEntity nType = 5 // random
    m_pRopeEntity nStatus = 6 // random

    IsEntityAttachedToCrane returned b78500
    IsRopeOwnedByCrane returned 1


    m_pRopeEntity - m_pRopeHolder = 29;
*/

void CRopesSA::DebugRope ( uchar ucRopeID )
{
    if ( ucRopeID >= MAX_ROPES )
        return;

    CRopeSAInterface * pRopeInterface = Ropes[ ucRopeID ]->GetInterface ( );

    g_pCore->GetConsole()->Printf("pRopeInterface = %x", pRopeInterface);

    g_pCore->GetConsole()->Printf("m_vecSegments[0] = %x, %x, %x", pRopeInterface->m_vecSegments[0].fX, pRopeInterface->m_vecSegments[0].fY, pRopeInterface->m_vecSegments[0].fZ);
    g_pCore->GetConsole()->Printf("m_vecSegmentsReleased[0] = %x, %x, %x", pRopeInterface->m_vecSegmentsReleased[0].fX, pRopeInterface->m_vecSegmentsReleased[0].fY, pRopeInterface->m_vecSegmentsReleased[0].fZ);

    g_pCore->GetConsole()->Printf("m_pRopeEntity = %x", pRopeInterface->m_pRopeEntity);
    g_pCore->GetConsole()->Printf("m_pad4 = %x", pRopeInterface->m_pad4);
    g_pCore->GetConsole()->Printf("m_fMass = %x", pRopeInterface->m_fMass);
    g_pCore->GetConsole()->Printf("m_fSegmentLength = %x", pRopeInterface->m_fSegmentLength);
    g_pCore->GetConsole()->Printf("m_pRopeHolder = %x", pRopeInterface->m_pRopeHolder);
    g_pCore->GetConsole()->Printf("m_pRopeAttacherEntity = %x", pRopeInterface->m_pRopeAttacherEntity);
    g_pCore->GetConsole()->Printf("m_pAttachedEntity = %x", pRopeInterface->m_pAttachedEntity);
    g_pCore->GetConsole()->Printf("m_uiHoldEntityExpireTime = %x", pRopeInterface->m_uiHoldEntityExpireTime);
    g_pCore->GetConsole()->Printf("m_ucSegmentCount = %x", pRopeInterface->m_ucSegmentCount);
    g_pCore->GetConsole()->Printf("m_ucRopeType = %x", pRopeInterface->m_ucRopeType);
    g_pCore->GetConsole()->Printf("m_ucFlags1 = %x", pRopeInterface->m_ucFlags1);
    g_pCore->GetConsole()->Printf("m_ucFlags2 = %x", pRopeInterface->m_ucFlags2);

    if ( pRopeInterface->m_pRopeEntity )
    {
        g_pCore->GetConsole()->Printf("m_pRopeEntity m_nModelIndex = %x", pRopeInterface->m_pRopeEntity->m_nModelIndex);
        g_pCore->GetConsole()->Printf("m_pRopeEntity nType = %x", pRopeInterface->m_pRopeEntity->nType);
        g_pCore->GetConsole()->Printf("m_pRopeEntity nStatus = %x", pRopeInterface->m_pRopeEntity->nStatus);


        g_pCore->GetConsole()->Printf("m_pRopeEntity bUsesCollision = %x", pRopeInterface->m_pRopeEntity->bUsesCollision);
        g_pCore->GetConsole()->Printf("m_pRopeEntity bIsStatic = %x", pRopeInterface->m_pRopeEntity->bIsStatic);
        g_pCore->GetConsole()->Printf("m_pRopeEntity bIsVisible = %x", pRopeInterface->m_pRopeEntity->bIsVisible);

        // Ask somethin
        DWORD dwThis = (DWORD) pRopeInterface->m_pRopeEntity;
        DWORD dwFunc = FUNC_CRope_IsEntityAttachedToCrane;
        DWORD dwReturn = -1;
        _asm
        {
            mov        ecx, dwThis
            call    dwFunc
            mov        dwReturn, eax
        }
        g_pCore->GetConsole()->Printf("IsEntityAttachedToCrane returned %x", dwReturn);

        dwThis = (DWORD) pRopeInterface;
        dwFunc = FUNC_CRope_IsRopeOwnedByCrane;
        dwReturn = -1;
        _asm
        {
            mov        ecx, dwThis
            call    dwFunc
            mov        dwReturn, eax
        }
        g_pCore->GetConsole()->Printf("IsRopeOwnedByCrane returned %x", dwReturn);
    }

    //CEntitySAInterface * pVehicleInterface = (CEntitySAInterface *)pGame->GetPedContext()->GetVehicle()->GetInterface();
    //g_pCore->GetConsole()->Printf("m_pRopeEntity pVehicleInterface m_pRwObject = %x", pVehicleInterface->m_pRwObject);
}

CRope * CRopesSA::CreateRope ( CEntity * pRopeEntity, CVector & vecPosition, uchar ucSegmentCount, CEntity * pRopeHolder )
{
    g_pCore->GetConsole()->Printf("CreateRope ucSegmentCount = %x", ucSegmentCount);
    DWORD dwFunc = FUNC_CRope_Create;

    // Entity which holds rope. Example leviathan. Works partially now - magnet has same Z velocity as this and this is downed up by attached entity (to magnet) mass.
    // But position isn't updating for some reason.
    CEntitySAInterface * pRopeHolderInterface = pRopeHolder->GetInterface();

    // ?
    DWORD dwRopeEntityInterface = (DWORD)pRopeHolder->GetInterface() + 29;

    CEntitySAInterface * pRopeEntityInterface = (CEntitySAInterface * ) dwRopeEntityInterface;

    //DWORD dwRopeHolderInterface = ( DWORD ) pRopeHolder->GetInterface ();
    //DWORD dwRopeEntityInterface = ( DWORD ) pRopeEntity->GetInterface ();
    
    //DWORD dwRopeHolderInterface = ( DWORD ) pRopeHolder;
    //DWORD dwRopeEntityInterface = ( DWORD ) pRopeEntity;

    DWORD dwReturn = 0;

    char ucRopeType = 1;
    bool bExpires = false; // make ropes never expiring because currently there no way to detect it; also not sure if it actually work (maybe only in certain types? swat rope?)
    char ucSegmentCountEx = ucSegmentCount;
    char ucFlags = 1;
    int uiExpireTime = 0; // is this even working?

    float fX = vecPosition.fX;
    float fY = vecPosition.fY;
    float fZ = vecPosition.fZ;

    //g_pCore->GetConsole()->Printf("CreateRope ucSegmentCount = %x; dwRopeHolderInterface = %x; dwRopeEntityInterface = %x", ucSegmentCount, dwRopeHolderInterface, dwRopeEntityInterface);

    _asm
    {
        push    uiExpireTime
        push    pRopeHolderInterface
        push    ucFlags
        push    ucSegmentCountEx
        push    bExpires
        push    fZ
        push    fY
        push    fX
        push    ucRopeType
        push    pRopeEntityInterface
        call    dwFunc
        add        esp, 4*10
        mov        dwReturn, eax
    }

    g_pCore->GetConsole()->Printf("CreateRope returned %x", dwReturn);

    if ( dwReturn == 0 )
        return ( CRope* ) NULL; // Limit reached

    CRopeSAInterface * pRopeInterface = ( CRopeSAInterface * ) FindByRopeEntity ( pRopeEntityInterface );
    //m_pInterface = ( CRopeSAInterface * ) ( ARRAY_CRopes + iRopeID * sizeof( CRopeSAInterface ) );

    if ( pRopeInterface->m_pRopeAttacherEntity )
    {
        // By default, most rope types create some directly attached object (in 0x556070 CRope::CreateRopeAttacherEntity) (magnet, wrecking ball etc.) so destroy it
        pGame->GetWorld ()->Remove ( pRopeInterface->m_pRopeAttacherEntity, CObject_Destructor );
    }
    
    // Do not expire plx. For some reason default 0 don't work.
    pRopeInterface->m_uiHoldEntityExpireTime = -1;

    // Try somethin
    //pRopeInterface->m_ucFlags2 = 4;

    for ( int i = 0; i < MAX_ROPES; i++ )
    {
        if ( Ropes [i]->GetInterface () == pRopeInterface )
        {
            DebugRope ( i );
            return ( CRope * ) Ropes [i];
        }
    }
    return ( CRope* ) NULL;
}

// Returns rope which use specified rope entity
//CRope * CRopesSA::FindByRopeEntity ( CEntity * pRopeEntity )
CRope * CRopesSA::FindByRopeEntity ( CEntitySAInterface * pRopeEntity )
{
    //CEntitySA * pRopeEntitySA = dynamic_cast < CEntitySA* > ( pRopeEntity );
    DWORD dwFunc = FUNC_CRope_FindByRopeEntity;
    //DWORD dwRopeEntityInterface = ( DWORD ) pRopeEntitySA->GetInterface ();
    DWORD dwRopeEntityInterface = ( DWORD ) pRopeEntity;
    int iReturn = -1;

    _asm
    {
        push    dwRopeEntityInterface
        call    dwFunc
        add        esp, 4
        mov        iReturn, eax // id (0 - 7)
    }
    if ( iReturn == -1 )
        return ( CRope * ) NULL;
    return ( CRope * ) ( ARRAY_CRopes + iReturn * sizeof( CRopeSAInterface ) );
}

void CRopesSA::ProcessAll ( void )
{
    DWORD dwFunc = FUNC_CRope_ProcessAll;

    _asm
    {
        call    dwFunc
    }
}
