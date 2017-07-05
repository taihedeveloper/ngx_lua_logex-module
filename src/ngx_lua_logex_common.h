/*
 * Copyright (C) Fangyu Xia (refirst)
 */

#ifndef _NGX_LUA_LOGEX_COMMON_H_INCLUDED_
#define _NGX_LUA_LOGEX_COMMON_H_INCLUDED_

#include <ngx_core.h>
#include <ngx_http.h>
#include <lauxlib.h>
#include "ngx_http_lua_api.h"

typedef struct ngx_lua_logex_s ngx_lua_logex_t;
typedef struct ngx_lua_logex_loc_conf_s ngx_lua_logex_loc_conf_t;

struct ngx_lua_logex_s
{
    ngx_str_t origal_name;
    uint32_t level;
    int32_t last_hour;
    int32_t log_fd;
    int32_t log_fd_wf;
};

struct ngx_lua_logex_loc_conf_s
{
    ngx_lua_logex_t* logex;
    ngx_str_t logex_file_path;
    ngx_uint_t logex_level;
};

#endif /* _NGX_LUA_LOGEX_COMMON_H_INCLUDED_ */

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
