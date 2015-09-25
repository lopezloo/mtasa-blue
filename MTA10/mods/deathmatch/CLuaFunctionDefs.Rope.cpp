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
    CClientEntity*  pRopeEntity;
    CVector            vecPosition;
    uchar            ucSegmentCount;
    CClientEntity*    pRopeHolder;

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
    CClientRope*    pRope;
    CClientEntity*    pRopeAttacher;

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
    CClientRope*    pRope;
    CClientEntity*    pAttachedEntity;

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
