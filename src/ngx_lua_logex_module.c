/*
 * Copyright (C) Fangyu Xia (refirst)
 */

#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"
#include "ngx_lua_logex_file.h"
#include "ngx_lua_logex_api.h"

static ngx_int_t ngx_lua_logex_post_conf(ngx_conf_t* cf);
static void* ngx_lua_logex_create_main_conf(ngx_conf_t* cf);
static void* ngx_lua_logex_create_loc_conf(ngx_conf_t* cf);
static char* ngx_lua_logex_merge_loc_conf(ngx_conf_t* cf, void* parent, void* child);
static ngx_int_t ngx_lua_logex_init_fd(ngx_conf_t* cf, ngx_lua_logex_loc_conf_t* llcf);
static int32_t ngx_lua_logex_inject_api(lua_State* L);

static ngx_command_t ngx_lua_logex_cmds[] = {
    {
        ngx_string("lua_logex_file_path"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_lua_logex_loc_conf_t, logex_file_path),
        NULL
    },

    {
        ngx_string("lua_logex_level"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_num_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_lua_logex_loc_conf_t, logex_level),
        NULL
    },

    ngx_null_command
};

ngx_http_module_t ngx_lua_logex_module_ctx = {
    NULL,                             /*  preconfiguration */
    ngx_lua_logex_post_conf,          /*  postconfiguration */
    ngx_lua_logex_create_main_conf,   /*  create main configuration */
    NULL,                             /*  init main configuration */
    NULL,                             /*  create server configuration */
    NULL,                             /*  merge server configuration */
    ngx_lua_logex_create_loc_conf,    /*  create location configuration */
    ngx_lua_logex_merge_loc_conf      /*  merge location configuration */
};

ngx_module_t ngx_lua_logex_module = {
    NGX_MODULE_V1,
    &ngx_lua_logex_module_ctx,  /*  module context */
    ngx_lua_logex_cmds,         /*  module directives */
    NGX_HTTP_MODULE,            /*  module type */
    NULL,                       /*  init master */
    NULL,                       /*  init module */
    NULL,                       /*  init process */
    NULL,                       /*  init thread */
    NULL,                       /*  exit thread */
    NULL,                       /*  exit process */
    NULL,                       /*  exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t ngx_lua_logex_post_conf(ngx_conf_t* cf)
{
    ngx_lua_logex_main_conf_t* lmcf;

    //get logex_id variable
    char* logexname = "logex_id";
    size_t logexlen = strlen(logexname);

    lmcf = ngx_http_conf_get_module_main_conf(cf, ngx_lua_logex_module);

    lmcf->logexid_name.data = (u_char*)ngx_pcalloc(cf->pool, logexlen + 1);
    ngx_memcpy(lmcf->logexid_name.data, logexname, logexlen);
    lmcf->logexid_name.len = logexlen;
    lmcf->logexid_hash = ngx_hash_key(lmcf->logexid_name.data, lmcf->logexid_name.len);

    //add current lua api into package by use ngx lua module, so use this module must dependent ngx lua module
    if (ngx_http_lua_add_package_preload(cf, "ngx.ext", ngx_lua_logex_inject_api) != NGX_OK)
    {
        return NGX_ERROR;
    }

    return NGX_OK;
}

static void* ngx_lua_logex_create_main_conf(ngx_conf_t* cf)
{
    ngx_lua_logex_main_conf_t* lmcf;
    lmcf = ngx_pcalloc(cf->pool, sizeof(ngx_lua_logex_main_conf_t));
    return lmcf;
}

static void* ngx_lua_logex_create_loc_conf(ngx_conf_t *cf)
{
    ngx_lua_logex_loc_conf_t* llcf;
    llcf = ngx_pcalloc(cf->pool, sizeof(ngx_lua_logex_loc_conf_t));
    if (llcf == NULL)
    {
        return NULL;
    }

    llcf->logex_level = NGX_CONF_UNSET_UINT;

    return llcf;
}

static char* ngx_lua_logex_merge_loc_conf(ngx_conf_t* cf, void* parent, void* child)
{
    ngx_lua_logex_loc_conf_t* prev = parent;
    ngx_lua_logex_loc_conf_t* llcf = child;

    ngx_conf_merge_str_value(llcf->logex_file_path, prev->logex_file_path, "");
    ngx_conf_merge_uint_value(llcf->logex_level, prev->logex_level, 2);

    //init file handler
    if (ngx_lua_logex_init_fd(cf, llcf) != NGX_OK)
    {
        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}

static ngx_int_t ngx_lua_logex_init_fd(ngx_conf_t* cf, ngx_lua_logex_loc_conf_t* llcf)
{
    if (llcf->logex_file_path.len > 0)
    {
        llcf->logex = ngx_pcalloc(cf->pool, sizeof(ngx_lua_logex_t));
        if (llcf->logex == NULL)
        {
            return NGX_ERROR;
        }

        //open log file
        if (ngx_lua_logex_open(cf, llcf->logex_file_path.data, llcf->logex_level, llcf->logex) < 0)
        {
            return NGX_ERROR;
        }
    }

    return NGX_OK;
}

static int32_t ngx_lua_logex_inject_api(lua_State* L)
{
    ngx_lua_logex_inject(L);
    return 1;
}

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
