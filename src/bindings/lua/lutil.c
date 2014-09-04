/*****************************************************************************\
 *  Copyright (c) 2014 Lawrence Livermore National Security, LLC.  Produced at
 *  the Lawrence Livermore National Laboratory (cf, AUTHORS, DISCLAIMER.LLNS).
 *  LLNL-CODE-658032 All rights reserved.
 *
 *  This file is part of the Flux resource manager framework.
 *  For details, see https://github.com/flux-framework.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the license, or (at your option)
 *  any later version.
 *
 *  Flux is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the terms and conditions of the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *  See also:  http://www.gnu.org/licenses/
\*****************************************************************************/

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <lua.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int lua_pusherror (lua_State *L, char *fmt, ...)
{
    char *msg;
    va_list ap;

    va_start (ap, fmt);
    vasprintf (&msg, fmt, ap);
    va_end (ap);

    lua_pushnil (L);
    lua_pushstring (L, msg);
    free (msg);
    return (2);
}

int l_pushresult (lua_State *L, int rc)
{
    if (rc < 0)
        return lua_pusherror (L, strerror (errno));
    lua_pushnumber (L, rc);
    return (1);
}

int l_loadlibrary (lua_State *L, const char *name)
{
    /* Equivalent of require(name) */
    lua_getglobal (L, "require");
    lua_pushstring (L, name);
    return lua_pcall (L, 1, 0, 0);
}

int l_format_args (lua_State *L, int index)
{
    int i;
    int t = lua_gettop (L);
    int nargs = t - index + 1;

    lua_getglobal (L, "string");
    lua_getfield (L, -1, "format");

    for (i = index; i <= t; i++)
        lua_pushvalue (L, i);

    if (lua_pcall (L, nargs, 1, 0) != 0) {
        lua_pusherror (L, "string.format: %s", lua_tostring (L, -1));
        return (-1);
    }
    lua_replace (L, index);
    lua_settop (L, index);
    return (1);
}


/*
 * vi: ts=4 sw=4 expandtab
 */