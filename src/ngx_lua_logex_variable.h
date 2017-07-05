/*
 * Copyright (C) Fangyu Xia (refirst)
 */

#ifndef _NGX_LUA_LOGEX_VARIABLE_H_INCLUDED_
#define _NGX_LUA_LOGEX_VARIABLE_H_INCLUDED_

#include "ngx_lua_logex_common.h"

ngx_int_t ngx_lua_logex_variables_init(ngx_conf_t* cf);
ngx_int_t ngx_lua_logex_add_variables(ngx_conf_t* cf);
uint32_t ngx_lua_logex_get_logex_id(ngx_http_request_t* r);

#endif /* _NGX_LUA_LOGEX_VARIABLE_H_INCLUDED_ */

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
