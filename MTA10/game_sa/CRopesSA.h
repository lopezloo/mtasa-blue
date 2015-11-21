/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRopesSA.h
*  PURPOSE:     Header file for rope manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               JoeBullet <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_ROPES
#define __CGAMESA_ROPES

#include <game/CRopes.h>
#include "CRopeSA.h"

#define ARRAY_CRopes                                0xB768B8
#define VAR_CRope_Controlled_Crane_Number           0xB76898

class CRopeSA;

class CRopesSA : public CRopes
{
private:
    CRopeSA*                    Ropes                       [ MAX_ROPES ];

public:
                                CRopesSA                    ( void );
                                ~CRopesSA                   ( void );

    CRope*                      CreateRope                  ( CVector & vecPosition, CEntity * pRopeHolder, uchar ucSegmentCount );
  //CRope*                      FindByRopeEntity            ( CEntity * pRopeEntity );
    CRope*                      FindByRopeEntity            ( CEntitySAInterface * pRopeEntity );

    int                         CreateRopeForSwatPed        ( const CVector & vecPosition, DWORD dwDuration = 4000 );

    void                        Update                      ( void );

    void                        DebugRope                   ( uchar ucRopeID );
};

#endif