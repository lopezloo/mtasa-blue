/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CRopes.h
*  PURPOSE:     Rope entity manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRopes_H
#define __CRopes_H

#include "Common.h"
#include "CRope.h"
#include <CVector.h>

#include <windows.h>

class CRopes
{
public:
    virtual CRope*      CreateRope                      ( CEntity * pRopeEntity, CVector & vecPosition, uchar ucSegmentCount, CEntity * pRopeHolder ) = 0;
    virtual int         CreateRopeForSwatPed            ( const CVector & vecPosition, DWORD dwDuration = 4000 ) = 0;
    virtual void        ProcessAll                      ( void ) = 0;

    virtual void        DebugRope                       ( uchar ucRopeID ) = 0;
};

#endif
