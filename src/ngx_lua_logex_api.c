/*
 * Copyright (C) Fangyu Xia (refirst)
 */

#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"
#include "ngx_lua_logex_api.h"
#include "ngx_lua_logex_file.h"
#include "ngx_lua_logex_variable.h"

ngx_module_t ngx_lua_logex_module;

static int32_t ngx_http_lua_logex(lua_State* L)
{
    LUA_INTEGER level;
    u_char* filename;
    LUA_INTEGER line;
    u_char* fmt;
    uint32_t logex_id;
    ngx_http_request_t* r;
    ngx_lua_logex_loc_conf_t* llcf;

    level = luaL_checkinteger(L, 1);
    filename = (u_char*)luaL_checklstring(L, 2, NULL);
    line = luaL_checkinteger(L, 3);
    fmt = (u_char*)luaL_checklstring(L, 4, NULL);
    r = ngx_http_lua_get_request(L);

    logex_id = ngx_lua_logex_get_logex_id(r);

    llcf = ngx_http_get_module_loc_conf(r, ngx_lua_logex_module);
    ngx_lua_logex_write(level, logex_id, (const char*)filename, line, (const char*)fmt, llcf->logex);
    return 0;
}

void ngx_lua_logex_inject(lua_State* L)
{
    lua_createtable(L, 0 /* narr */, 1 /* nrec */);    /* .req */

    lua_pushcfunction(L, ngx_http_lua_logex);
    lua_setfield(L, -2, "log");
}

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
