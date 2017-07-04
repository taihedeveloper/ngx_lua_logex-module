/*
 * Copyright (C) Fangyu Xia (refirst)
 */

#ifndef _NGX_LUA_LOGEX_FILE_H_INCLUDED_
#define _NGX_LUA_LOGEX_FILE_H_INCLUDED_

#include "ngx_lua_logex_common.h"

int32_t ngx_lua_logex_open(ngx_conf_t* cf, const u_char* name, uint32_t level, ngx_lua_logex_t* t);
int32_t ngx_lua_logex_write(uint32_t level, uint32_t logid, const char* filename, uint32_t line, const char* fmt, ngx_lua_logex_t* t);

#endif /* _NGX_LUA_LOGEX_FILE_H_INCLUDED_ */

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
