/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRopeSA.h
*  PURPOSE:     Header file for rope entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*                JoeBullet <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRopeSA_H
#define __CRopeSA_H

#include <game/CRope.h>
#include "CEntitySA.h"

#define FUNC_CRope_ResetAll                         0x555DC0
#define FUNC_CRope_IsEntityAttachedToCrane          0x555F80
#define FUNC_CRope_IsRopeOwnedByCrane               0x555FB0
#define FUNC_CRope_FindByRopeEntity                 0x556000
#define FUNC_CRope_ReleasePickedUpObject            0x556030
#define FUNC_CRope_CreateRopeAttacherObject         0x556070
#define FUNC_CRope_Destructor                       0x556780
#define FUNC_CRope_Draw                             0x556800
#define FUNC_CRope_AttachEntity                     0x5569C0
#define FUNC_CRope_DrawAll                          0x556AE0
#define FUNC_CRope_DestroyAll                       0x556B10
#define FUNC_CRope_Create                           0x556B40
#define FUNC_CRope_Process                          0x557530
#define FUNC_CRope_CreateRopeForSwatPed             0x558d10
#define FUNC_CRope_ProcessAll                       0x558D70
#define FUNC_CRope_Adjust                           0x5561B0

class CRopeSAInterface
{
public:
    CVector m_vecSegments[32];
    CVector m_vecSegmentsReleased[32];
    CEntitySAInterface * m_pRopeEntity;
    float m_fGroundHeight;
    float m_fMass;
    float m_fSegmentLength;
    CEntitySAInterface * m_pRopeHolder; // leviathan
    CEntitySAInterface * m_pRopeAttacherEntity; // magnet ; m_pRopeAttacherObject
    CEntitySAInterface * m_pAttachedEntity;        // entity attached to magnet
    float m_uiRopeSegmentLength;
    uint32 m_uiHoldEntityExpireTime;
    uint8 m_ucSegmentCount;
    uint8 m_ucRopeType;

    //uint8 m_ucFlags1;
    unsigned char m_bFlags1_1 : 1;
    unsigned char m_bFlags1_2 : 1;
    unsigned char m_bFlags1_3 : 1;
    unsigned char m_bFlags1_4 : 1; // 1 after destroying heli with winch
    unsigned char m_bFlags1_5 : 1; // 1 after destroying heli with winch
    unsigned char m_bFlags1_6 : 1; // 1 after destroying heli with winch
    unsigned char m_bFlags1_7 : 1;
    unsigned char m_bFlags1_8 : 1;

    //uint8 m_ucFlags2;
    unsigned char m_bFlags2_1 : 1;
    unsigned char m_bFlags2_2 : 1;
    unsigned char m_bFlags2_3 : 1;
    unsigned char m_bFlags2_4 : 1;
    unsigned char m_bFlags2_5 : 1;
    unsigned char m_bFlags2_6 : 1;
    unsigned char m_bFlags2_7 : 1;
    unsigned char m_bFlags2_8 : 1;
};
C_ASSERT(sizeof(CRopeSAInterface) == 0x328);

class CRopeSA : public CRope
{
private:
    CRopeSAInterface*        m_pInterface;

public:
                            CRopeSA                         ( CRopeSAInterface * pRopeInterface );
                          //~CRopeSA                        ( );

    CRopeSAInterface*       GetInterface                    ()    { return ( CRopeSAInterface * ) m_pInterface; }
    
    void                    Remove                          ( void );

    void                    SetAttacherEntity               ( CEntity * pRopeAttacherEntity );
    void                    SetAttachedEntity               ( CEntity * pEntityToAttach );

    unsigned char           GetSegmentCount                 ( void );
    void                    SetSegmentCount                 ( uchar ucSegmentCount );

    CVector                 GetSegmentPosition              ( uchar ucSegment );
    void                    SetSegmentPosition              ( uchar ucSegment, CVector & vecPosition );

    float                   GetSegmentLength                ( void );
    void                    SetSegmentLength                ( float fLength );

    void                    ReleasePickedUpObject           ( void );
    void                    Adjust                          ( const CVector & vecPosition );
    bool                    IsRopeOwnedByCrane              ( void );
};

#endif