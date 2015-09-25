/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRope.h
*  PURPOSE:     Rope entity class
*
*****************************************************************************/


#ifndef __CCLIENTROPE_H
#define __CCLIENTROPE_H

//#include "CClientStreamElement.h"
#include "CClientCommon.h"
#include "CClientEntity.h"

class CClientRopeManager;

//class CClientRope : public CClientEntity
class CClientRope : public CClientStreamElement
{
    //DECLARE_CLASS ( CClientRope, CClientEntity )
    DECLARE_CLASS ( CClientRope, CClientStreamElement )
    friend class CClientRopeManager;

public:
                                CClientRope                ( CClientManager * pManager, ElementID ID, CClientEntity * pRopeEntity, CVector vecPosition, uchar ucSegments, CClientEntity * pRopeHolder );
                                ~CClientRope            ( void );

    void                        Unlink                  ( void );

    inline eClientEntityType    GetType                 ( void ) const                      { return CCLIENTROPE; };
    inline CRope *              GetGameRope                ( void )                            { return m_pRope; }


    void                        SetSegmentPosition        ( uchar ucSegment, CVector vecPosition );
    CVector                        GetSegmentPosition        ( uchar ucSegment );
    
    inline const CVector&       GetPosition             ( void )                            { return m_vecSegmentsPosition[0]; };
    inline void                 GetPosition             ( CVector& vecPosition ) const      { vecPosition = m_vecSegmentsPosition[0]; };
    void                        SetPosition             ( const CVector& vecPosition );

    void                        SetSegmentCount            ( uchar ucSegmentCount );
    uchar                        GetSegmentCount            ( void );

    void                        SetSegmentLength        ( float fSegmentLength );
    float                        GetSegmentLength        ( void );

    void                        SetAttacherEntity        ( CClientEntity * pRopeAttacherEntity );
    void                        SetAttachedEntity        ( CClientEntity * pEntityToAttach );    

    bool                        DetachElementFromRope    ( CClientEntity * pAttachedElement );

protected:
    void                        StreamIn                ( bool bInstantly );
    void                        StreamOut               ( void );

    void                        Create                  ( void );
    void                        Destroy                 ( void );

    void                        NotifyCreate            ( void );
    void                        NotifyDestroy           ( void );

    void                        StreamedInPulse         ( void );

    class CClientRopeManager*    m_pRopeManager;
    CVector                        m_vecSegmentsPosition[32];
    //CVector                        m_vecSegmentsReleased[32];
    CClientEntity*                m_pRopeAttacherEntity;
    CClientEntity*                m_pAttachedEntity;

    uchar                        m_ucSegmentCount;
    float                        m_fSegmentLength;

    CClientEntity*                m_pRopeEntity;
    CClientEntity*                m_pRopeHolder;

    bool                        m_bStreamedIn;

public:
    CRope*                        m_pRope;
};

#endif
