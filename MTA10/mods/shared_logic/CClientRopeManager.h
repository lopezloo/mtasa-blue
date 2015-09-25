/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRopeManager.h
*  PURPOSE:     Rope entity manager class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

class CClientRopeManager;

#ifndef __CCLIENTROPEMANAGER_H
#define __CCLIENTROPEMANAGER_H

#include "CClientManager.h"
#include "CClientRope.h"
#include <list>

class CClientRopeManager
{

public:
    ZERO_ON_NEW
                                                            CClientRopeManager            ( CClientManager * pManager );
                                                            ~CClientRopeManager            ( void );

    void                                                    DoPulse                     ( void );
    void                                                    DeleteAll                   ( void );

    inline unsigned int                                        Count                       ( void )                        { return static_cast < unsigned int > ( m_Ropes.size () ); };

    
    CClientRope*                                            Get                         ( ElementID ID );
    //CClientRope*                                            Get                         ( CRope* pRope, bool bValidatePointer );

    static bool                                                IsRopeLimitReached            ( void );

    inline void                                                AddToList                   ( CClientRope * pRope )          { m_List.push_back ( pRope ); };
    void                                                    RemoveFromLists                ( CClientRope * pRope );

    void                                                    OnCreation                  ( CClientRope * pRope );
    void                                                    OnDestruction               ( CClientRope * pRope );

private:

    void                                                    UpdateLimitInfo             ( void );

    CClientManager*                                            m_pManager;

    std::list < CClientRope* >                                m_List;
    bool                                                    m_bCanRemoveFromList;
    CMappedArray < CClientRope* >                            m_Ropes;
    CMappedArray < CClientRope* >                            m_StreamedIn;

public:
    static unsigned int                                        m_uiStreamedInCount;
};

#endif
