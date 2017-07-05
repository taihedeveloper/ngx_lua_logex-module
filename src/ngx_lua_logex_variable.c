/*
 * Copyright (C) Fangyu Xia (refirst)
 */

#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"
#include "ngx_lua_logex_variable.h"

#define LOGEX_RAND_RANGE 1000
#define LOGEX_ID_LENGTH 11

ngx_module_t ngx_lua_logex_module;
static ngx_int_t _logex_id_index = -1;

static ngx_int_t ngx_lua_get_variable_logex_id(ngx_http_request_t* r, ngx_http_variable_value_t* v, uintptr_t data);

static ngx_http_variable_t ngx_lua_logex_variables[] = {
    {
        ngx_string("logex_id"),
        0,
        ngx_lua_get_variable_logex_id,
        0,
        NGX_HTTP_VAR_NOCACHEABLE|NGX_HTTP_VAR_NOHASH,
        0
    },

    {ngx_null_string, NULL, NULL, 0, 0, 0}
};

static ngx_int_t ngx_lua_get_variable_logex_id(ngx_http_request_t* r, ngx_http_variable_value_t* v, uintptr_t data)
{
    struct timeval tv = {0, 0};
    char* logex_id = NULL;
    ngx_uint_t logex_id_real_len = 0;
    ngx_int_t random_number = 0;
    ngx_lua_logex_loc_conf_t* llcf = NULL;

    llcf = ngx_http_get_module_loc_conf(r, ngx_lua_logex_module);

    if (r->variables[_logex_id_index].data != NULL)
    {
        if (r->variables[_logex_id_index].len > LOGEX_ID_LENGTH)
        {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "logex_id len error");
            return NGX_ERROR;
        }

        return NGX_OK;
    }
    else
    {
        logex_id = (char*)ngx_pcalloc(r->connection->pool, LOGEX_ID_LENGTH);
        if (logex_id == NULL)
        {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "logex_id alloc failed ");
            return NGX_ERROR;
        }

        ngx_gettimeofday(&tv);
        random_number = random() / (RAND_MAX + 1.0) * LOGEX_RAND_RANGE;
        ngx_snprintf((u_char*)logex_id, LOGEX_ID_LENGTH, "%04d%03d%03d", tv.tv_sec % 3600, tv.tv_usec / 1000, random_number);

        logex_id_real_len = strlen((char*)logex_id);
        if (logex_id_real_len >= LOGEX_ID_LENGTH)
        {
            return NGX_ERROR;
        }

        r->variables[_logex_id_index].len= logex_id_real_len;
        r->variables[_logex_id_index].valid = 1;
        r->variables[_logex_id_index].not_found = 0;
        r->variables[_logex_id_index].no_cacheable = 0;
        r->variables[_logex_id_index].data = (u_char*)logex_id;
    }

    return NGX_OK;
}

ngx_int_t ngx_lua_logex_variables_init(ngx_conf_t* cf)
{
    ngx_str_t logex_id = ngx_string("logex_id");
    _logex_id_index = ngx_http_get_variable_index(cf, &logex_id);

    if (_logex_id_index == NGX_ERROR)
    {
        ngx_log_error(NGX_LOG_ERR, cf->log, 0, "get logex_id index failed!");
        return NGX_ERROR;
    }

    return NGX_OK;
}

ngx_int_t ngx_lua_logex_add_variables(ngx_conf_t* cf)
{
    ngx_http_variable_t* var;
    ngx_http_variable_t* v;

    for (v = ngx_lua_logex_variables; v->name.len; v++)
    {
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == NULL)
        {
            return NGX_ERROR;
        }

        var->get_handler = v->get_handler;
        var->data = v->data;
    }

    return NGX_OK;
}

uint32_t ngx_lua_logex_get_logex_id(ngx_http_request_t* r)
{
    uint32_t logex_id = 0;

    ngx_variable_value_t* v = ngx_http_get_flushed_variable(r, _logex_id_index);
    if (v != NULL && v->data != NULL)
    {
        if (v->len >= LOGEX_ID_LENGTH)
        {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "logex_id len error");
            return logex_id;
        }

        logex_id = ngx_atoi(v->data, v->len);
    }

    return logex_id;
}

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
