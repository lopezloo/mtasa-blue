/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRopesSA.cpp
*  PURPOSE:     Rope manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               JoeBullet <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
DWORD dwDurationAddress = 0x558D1El;

int CRopesSA::CreateRopeForSwatPed ( const CVector & vecPosition, DWORD dwDuration )
{
    int iReturn;
    DWORD dwFunc = FUNC_CRopes_CreateRopeForSwatPed;
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
    pRopeInterface = b768b8
    m_vecSegments[0] = 21.641762, -7.014325, 2.683366
    m_vecSegmentsReleased[0] = 0.000000, 0.000000, -0.000750
    m_pRopeEntity = 1632907d
    m_pad4 = 2.117188 // looks like ground Z | getGroundPosition(21.641762, -7.014325, 2.683366) = 2.1171875
    m_fMass = 20.000000
    m_fSegmentLength = 0.645161
    m_pRopeHolder = 16329060 // leviathan
    m_pRopeAttacherEntity = 1636fc6c // magnet
    m_pad5 = 0.840000 // changes when pressing NUM8/2 | min = 0.01 = long rope | max = 0.84 = short rope
    m_pAttachedEntity = 0 // attached entity
    m_uiHoldEntityExpireTime = 0
    m_ucSegmentCount = 26 // changes when pressing NUM8/2 | min = 0 = short rope; max = 26 = long rope
    m_ucRopeType = 1
    m_ucFlags1_1 = 0
    m_ucFlags2_2 = 0
    m_ucFlags3_3 = 0
    m_ucFlags4_4 = 0
    m_ucFlags5_5 = 0
    m_ucFlags6_6 = 0
    m_ucFlags7_7 = 0
    m_ucFlags8_8 = 0
    m_ucFlags2_1 = 0
    m_ucFlags2_2 = 0
    m_ucFlags2_3 = 1
    m_ucFlags2_4 = 0
    m_ucFlags2_5 = 0
    m_ucFlags2_6 = 0
    m_ucFlags2_7 = 0
    m_ucFlags2_8 = 0
    m_pRopeEntity m_nModelIndex = 200
    m_pRopeEntity nType = 3
    m_pRopeEntity nStatus = 16
    m_pRopeEntity bUsesCollision = 0
    m_pRopeEntity bIsStatic = 0
    m_pRopeEntity bIsVisible = 0
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

    g_pCore->GetConsole()->Printf("m_vecSegments[0] = %f, %f, %f", pRopeInterface->m_vecSegments[0].fX, pRopeInterface->m_vecSegments[0].fY, pRopeInterface->m_vecSegments[0].fZ);
    g_pCore->GetConsole()->Printf("m_vecSegmentsReleased[0] = %f, %f, %f", pRopeInterface->m_vecSegmentsReleased[0].fX, pRopeInterface->m_vecSegmentsReleased[0].fY, pRopeInterface->m_vecSegmentsReleased[0].fZ);

    g_pCore->GetConsole()->Printf("m_pRopeEntity = %x", pRopeInterface->m_pRopeEntity);
    g_pCore->GetConsole()->Printf("m_pad4 = %f", pRopeInterface->m_fGroundHeight);
    g_pCore->GetConsole()->Printf("m_fMass = %f", pRopeInterface->m_fMass);
    g_pCore->GetConsole()->Printf("m_fSegmentLength = %f", pRopeInterface->m_fSegmentLength);
    g_pCore->GetConsole()->Printf("m_pRopeHolder = %x", pRopeInterface->m_pRopeHolder);
    g_pCore->GetConsole()->Printf("m_pRopeAttacherEntity = %x", pRopeInterface->m_pRopeAttacherEntity);
    g_pCore->GetConsole()->Printf("m_pad5 = %f", pRopeInterface->m_uiRopeSegmentLength);
    g_pCore->GetConsole()->Printf("m_pAttachedEntity = %x", pRopeInterface->m_pAttachedEntity);
    g_pCore->GetConsole()->Printf("m_uiHoldEntityExpireTime = %d", pRopeInterface->m_uiHoldEntityExpireTime);
    g_pCore->GetConsole()->Printf("m_ucSegmentCount = %d", pRopeInterface->m_ucSegmentCount);
    g_pCore->GetConsole()->Printf("m_ucRopeType = %d", pRopeInterface->m_ucRopeType);
    //g_pCore->GetConsole()->Printf("m_ucFlags1 = %x", pRopeInterface->m_ucFlags1);
    //g_pCore->GetConsole()->Printf("m_ucFlags2 = %x", pRopeInterface->m_ucFlags2);

    g_pCore->GetConsole()->Printf("m_ucFlags1_1 = %x", pRopeInterface->m_bFlags1_1);
    g_pCore->GetConsole()->Printf("m_ucFlags1_2 = %x", pRopeInterface->m_bFlags1_2);
    g_pCore->GetConsole()->Printf("m_ucFlags1_3 = %x", pRopeInterface->m_bFlags1_3);
    g_pCore->GetConsole()->Printf("m_ucFlags1_4 = %x", pRopeInterface->m_bFlags1_4);
    g_pCore->GetConsole()->Printf("m_ucFlags1_5 = %x", pRopeInterface->m_bFlags1_5);
    g_pCore->GetConsole()->Printf("m_ucFlags1_6 = %x", pRopeInterface->m_bFlags1_6);
    g_pCore->GetConsole()->Printf("m_ucFlags1_7 = %x", pRopeInterface->m_bFlags1_7);
    g_pCore->GetConsole()->Printf("m_ucFlags1_8 = %x", pRopeInterface->m_bFlags1_8);

    g_pCore->GetConsole()->Printf("m_ucFlags2_1 = %x", pRopeInterface->m_bFlags2_1);
    g_pCore->GetConsole()->Printf("m_ucFlags2_2 = %x", pRopeInterface->m_bFlags2_2);
    g_pCore->GetConsole()->Printf("m_ucFlags2_3 = %x", pRopeInterface->m_bSegmentGroundCheck);
    g_pCore->GetConsole()->Printf("m_ucFlags2_4 = %x", pRopeInterface->m_bFlags2_4);
    g_pCore->GetConsole()->Printf("m_ucFlags2_5 = %x", pRopeInterface->m_bFlags2_5);
    g_pCore->GetConsole()->Printf("m_ucFlags2_6 = %x", pRopeInterface->m_bFlags2_6);
    g_pCore->GetConsole()->Printf("m_ucFlags2_7 = %x", pRopeInterface->m_bFlags2_7);
    g_pCore->GetConsole()->Printf("m_ucFlags2_8 = %x", pRopeInterface->m_bFlags2_8);

    if ( pRopeInterface->m_pRopeEntity )
    {
        // Ask somethin
        DWORD dwThis = (DWORD) pRopeInterface->m_pRopeEntity;
        DWORD dwFunc = FUNC_CRopes_IsCarriedByRope;
        DWORD dwReturn = -1;
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
            mov     dwReturn, eax
        }
        g_pCore->GetConsole()->Printf("IsEntityAttachedToCrane returned %x", dwReturn);

        g_pCore->GetConsole()->Printf("IsRopeOwnedByCrane returned %x", Ropes [ ucRopeID ]->IsRopeOwnedByCrane());
    }

    //CEntitySAInterface * pVehicleInterface = (CEntitySAInterface *)pGame->GetPedContext()->GetVehicle()->GetInterface();
    //g_pCore->GetConsole()->Printf("m_pRopeEntity pVehicleInterface m_pRwObject = %x", pVehicleInterface->m_pRwObject);

    /*g_pCore->GetConsole()->Print("Rope segments");
    for ( int i = 0; i < 32; i++ )
    {
        g_pCore->GetConsole()->Printf( "m_vecSegments[%d] = %f, %f, %f", i, pRopeInterface->m_vecSegments[i].fX, pRopeInterface->m_vecSegments[i].fY, pRopeInterface->m_vecSegments[i].fZ );
        g_pCore->GetConsole()->Printf( "m_vecSegmentsReleased[%d] = %f, %f, %f", i, pRopeInterface->m_vecSegmentsReleased[i].fX, pRopeInterface->m_vecSegmentsReleased[i].fY, pRopeInterface->m_vecSegmentsReleased[i].fZ );        
    }*/

    g_pCore->GetConsole()->Printf("Crane array test");
    g_pCore->GetConsole()->Printf( "%d", (int *) (0xB76BB8 + ucRopeID * 0x328) );
    g_pCore->GetConsole()->Printf( "%d", (int *) (0xB76BB8 + ucRopeID * 0x328 + 4) );
    g_pCore->GetConsole()->Printf( "%d", (int *) (0xB76BB8 + ucRopeID * 0x328 + 8) );
}

CRope * CRopesSA::CreateRope ( CVector & vecPosition, CEntity * pRopeHolder, uchar ucSegmentCount )
{
    g_pCore->GetConsole()->Printf("CreateRope ucSegmentCount = %x", ucSegmentCount);

    // Find slot.
    uchar ucFreeSlot = 0;
    bool bFoundFreeSlot = false;
    while ( ucFreeSlot < MAX_ROPES && Ropes[ ucFreeSlot ]->GetInterface()->m_ucRopeType != 0 )
    {
        ucFreeSlot++;
    }
    if ( ucFreeSlot < MAX_ROPES )
        bFoundFreeSlot = true;

    if ( !bFoundFreeSlot )
        return NULL; // Limit reached

    DWORD dwFunc = FUNC_CRopes_RegisterRope;
    CRopeSAInterface * pRopeInterface = ( CRopeSAInterface * ) ( ARRAY_CRopes + ucFreeSlot * sizeof(CRopeSAInterface) );

    // Entity which holds rope. Example leviathan. Works partially now - magnet has same Z velocity as this and this is downed up by attached entity (to magnet) mass.
    // Velocity isn't updating properly (is extremaly high and lags) if holder is object.

    // But position isn't updating for some reason.
    // Pos update work if leviathan winch was overwritted by our rope (/createwinch + createRope())
    // 2669
    // container blue: 2932
    // INDUS_MAGNET (ROPE_INDUSTRIAL): 3053
    // MINI_MAGNET (ROPE_MINIMAGNET): 3056
    // heli_magnet (unused): 1301
    // 1393 - cool rope holder
    // 417 - leviathan

    CEntitySAInterface * pRopeHolderInterface = pRopeHolder->GetInterface();
    DWORD dwRopeEntityInterface = ( DWORD ) pRopeHolderInterface + 29;
    CEntitySAInterface * pRopeEntityInterface = ( CEntitySAInterface * ) dwRopeEntityInterface;

    bool bReturn = false;
    // Create rope as SWAT rope so it won't have problems with last segment if there is no magnet attached
    char ucRopeType = ROPE_SWAT;
    bool bExpires = false;
    
    // NOT A SEGMENT COUNT. Rope always have 32 segments.
    // m_ucSegmentCount + 1 segments become dynamic
    // Segments between 0 <-> m_ucSegmentCount are static line.
    //char ucSegmentCountEx = ucSegmentCount;

    char ucFlags = 1;
    int uiExpireTime = 0;
    float fX = vecPosition.fX;
    float fY = vecPosition.fY;
    float fZ = vecPosition.fZ;

    _asm
    {
        push    uiExpireTime
        push    pRopeHolderInterface
        push    ucFlags
        push    ucSegmentCount
        push    bExpires
        push    fZ
        push    fY
        push    fX
        push    ucRopeType
        push    pRopeEntityInterface
        call    dwFunc
        add     esp, 4*10
        mov     bReturn, al
    }
    g_pCore->GetConsole()->Printf( "CreateRope returned %d", bReturn );

    if ( !bReturn )
        return NULL; // Limit reached

    /*if ( ucRopeType < ROPE_SWAT && pRopeInterface->m_pRopeAttacherEntity )
    {
        // By default, most rope types create some directly attached object (in 0x556070 CRope::CreateRopeAttacherEntity) (magnet, wrecking ball etc.) so destroy it
        pGame->GetWorld ()->Remove ( pRopeInterface->m_pRopeAttacherEntity, CObject_Destructor );
        // Last (31) segment become static in that case.
    }*/
    
    // Do not expire plx. For some reason default 0 don't work.
    pRopeInterface->m_uiHoldEntityExpireTime = -1;
    pRopeInterface->m_bSegmentGroundCheck = true;

    DebugRope ( ucFreeSlot );

    SetSpeedOfTopNode ( ucFreeSlot, CVector(0, 0, -1) );

    return ( CRope * ) Ropes [ ucFreeSlot ];
}

// Returns rope which use specified rope entity
//CRope * CRopesSA::FindByRopeEntity ( CEntity * pRopeEntity )
CRope * CRopesSA::FindByRopeEntity ( CEntitySAInterface * pRopeEntity )
{
    //CEntitySA * pRopeEntitySA = dyfnamic_cast < CEntitySA* > ( pRopeEntity );
    DWORD dwFunc = FUNC_CRopes_FindRope;
    //DWORD dwRopeEntityInterface = ( DWORD ) pRopeEntitySA->GetInterface ();
    DWORD dwRopeEntityInterface = ( DWORD ) pRopeEntity;
    int iReturn = -1;

    _asm
    {
        push    dwRopeEntityInterface
        call    dwFunc
        add     esp, 4
        mov     iReturn, eax // id (0 - 7)
    }
    if ( iReturn == -1 )
        return NULL;
    return ( CRope * ) ( ARRAY_CRopes + iReturn * sizeof( CRopeSAInterface ) );
}

// Updates all ropes
void CRopesSA::Update ( void )
{
    DWORD dwFunc = FUNC_CRopes_Update;
    _asm
    {
        call    dwFunc
    }
}

// Untested. Change uiRope to CRope later
void CRopesSA::SetSpeedOfTopNode ( unsigned int uiRope, CVector vecSpeed )
{
    DWORD dwFunc = FUNC_CRopes_SetSpeedOfTopNode;
    DWORD dwReturn = 0;
    float fX = vecSpeed.fX;
    float fY = vecSpeed.fY;
    float fZ = vecSpeed.fZ;
    _asm
    {
        push    fZ
        push    fY
        push    fX
        push    uiRope
        call    dwFunc
        add     esp, 4*4
        mov     dwReturn, eax
    }

    g_pCore->GetConsole()->Printf("SetSpeedOfTopNode returned %d", dwReturn);
}