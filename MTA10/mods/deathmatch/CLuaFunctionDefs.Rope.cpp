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
// bool CreateRope ( float posX, float posY, float posZ [, element holder = nil, int segmentCount = 0 ] )
    CVector         vecPosition;
    uchar           ucSegmentCount;
    CClientEntity*  pRopeHolder;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadUserData ( pRopeHolder );
    argStream.ReadNumber   ( ucSegmentCount, 0 );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                CClientRope* pRope = CStaticFunctionDefinitions::CreateRope ( *pResource, vecPosition, pRopeHolder, ucSegmentCount );
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
    argStream.ReadUserData ( pRopeAttacher, NULL );

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

int CLuaFunctionDefs::AttachElementToRope ( lua_State* luaVM )
{
// bool AttachElementToRope ( element theElement )
    CClientRope*    pRope;
    CClientEntity*  pEntityToAttach;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRope );
    argStream.ReadUserData ( pEntityToAttach, NULL );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::AttachElementToRope ( pRope, pEntityToAttach ) )
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
// float, float, float getRopeSegmentPosition ( rope theRope, int segment ) 
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

int CLuaFunctionDefs::SetRopeSegmentLength ( lua_State* luaVM )
{
// bool setRopeSegmentLength ( rope theRope, float length ) 
    CClientRope*    pRope;
    float           fLength;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRope );
    argStream.ReadNumber ( fLength );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetRopeSegmentLength ( pRope, fLength ) )
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

int CLuaFunctionDefs::GetRopeSegmentLength ( lua_State* luaVM )
{
// float getRopeSegmentLength ( rope theRope ) 
    CClientRope*    pRope;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRope );

    if ( !argStream.HasErrors () )
    {
        float fLength;
        if ( CStaticFunctionDefinitions::GetRopeSegmentLength ( pRope, fLength ) )
        {
            lua_pushnumber ( luaVM, fLength );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}
