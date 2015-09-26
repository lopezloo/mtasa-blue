/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Pickup.cpp
*  PURPOSE:     Lua function definitions class
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::CreateRope ( lua_State * luaVM )
{
// bool CreateRope ( )
    CClientEntity*  pRopeEntity;
    CVector         vecPosition;
    uchar           ucSegmentCount;
    CClientEntity*  pRopeHolder;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRopeEntity );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber   ( ucSegmentCount );
    argStream.ReadUserData ( pRopeHolder );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                CClientRope* pRope = CStaticFunctionDefinitions::CreateRope ( *pResource, pRopeEntity, vecPosition, ucSegmentCount, pRopeHolder );
                if ( pRope )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CClientEntity* ) pRope );
                    }

                    // Return the handle
                    lua_pushelement ( luaVM, pRope );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::AttachElementToRopeAsAttacher ( lua_State* luaVM )
{
// bool AttachElementToRopeAsAttacher ( element theElement )
    CClientRope*    pRope;
    CClientEntity*  pRopeAttacher;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRope );
    argStream.ReadUserData ( pRopeAttacher );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::AttachElementToRopeAsAttacher ( pRope, pRopeAttacher ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::DetachElementFromRope ( lua_State* luaVM )
{
// bool DetachElementFromRope ( element theElement )
    CClientRope*    pRope;
    CClientEntity*  pAttachedEntity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRope );
    argStream.ReadUserData ( pAttachedEntity );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::DetachElementFromRope ( pRope, pAttachedEntity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetRopeSegmentPosition ( lua_State* luaVM )
{
// bool setRopeSegmentPosition ( rope theRope, int segment, float posX, float posY, float posZ ) 
    CClientRope*    pRope;
    uchar           ucSegment;
    CVector         vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRope );
    argStream.ReadNumber ( ucSegment );
    argStream.ReadVector3D ( vecPosition );

    if ( !argStream.HasErrors () )
    {
        if ( pRope->SetSegmentPosition ( ucSegment, vecPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetRopeSegmentPosition ( lua_State* luaVM )
{
// float posX, float posY, float posZ getRopeSegmentPosition ( rope theRope, int segment ) 
    CClientRope*    pRope;
    uchar           ucSegment;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRope );
    argStream.ReadNumber ( ucSegment );

    if ( !argStream.HasErrors () )
    {
        CVector vecPosition;
        if ( pRope->GetSegmentPosition ( ucSegment, vecPosition ) )
        {
            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            lua_pushnumber ( luaVM, vecPosition.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}
