/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CRope.h
*  PURPOSE:     Rope entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRope_H
#define __CRope_H

#define MAX_ROPES            8
#define MAX_ROPE_SEGMENTS    32

#include <windows.h>
#include <CVector.h>
#include "Common.h"

class CRope
{
public:
    virtual void        Remove                        ( void ) = 0;

    virtual CVector        GetSegmentPosition            ( uchar ucSegment ) = 0;
    virtual    void        SetSegmentPosition            ( uchar ucSegment, CVector & vecPosition ) = 0;

    virtual void        SetSegmentLength            ( float fSegmentLength ) = 0;
    virtual void        SetSegmentCount                ( uchar ucSegmentCount ) = 0;

    virtual void        SetAttacherEntity            ( CEntity * pRopeAttacherEntity ) = 0;
    virtual void        SetAttachedEntity            ( CEntity * pEntityToAttach ) = 0;

    virtual void        Adjust                        ( const CVector & vecPosition ) = 0;
};

#endif
