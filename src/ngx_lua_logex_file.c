/*
 * Copyright (C) Fangyu Xia (refirst)
 */

#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"
#include <dirent.h>
#include <sys/stat.h>
#include "ngx_lua_logex_file.h"

#define COMLOGEX_DEBUG 0
#define COMLOGEX_TRACE 1
#define COMLOGEX_NOTICE 2
#define COMLOGEX_WARNING 3
#define COMLOGEX_FATAL 4

static char* s_log_level[] = {
    "DEBUG  ",
    "TRACE  ",
    "NOTICE ",
    "WARNING",
    "FATAL  "
};

static void get_logex_time(ngx_tm_t *tm)
{
    time_t s;
    s = time(0);
    ngx_localtime(s, tm);
}

static int32_t create_logex_directory(char* filename)
{
    uint32_t count = 0;
    char* p = strchr(filename + 1, '/');

    while (p)
    {
        *p = 0;
        if (-1 == access(filename, F_OK))
        {
            if (-1 == mkdir(filename, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
            {
                return (-1);
            }
        }
        *p = '/';
        p = strchr(p + 1, '/');
        count++;
        if (count > 100)
        {
            return (-1);
        }
    }
    return 0;
}

int32_t ngx_lua_logex_open(ngx_conf_t* cf, const u_char* name, uint32_t level, ngx_lua_logex_t* t)
{
    u_char fullname[2048] = { 0 };
    size_t size = 0;
    if (NULL == name || NULL == t)
    {
        return (-1);
    }
    size = strlen((const char*)name);
    if (0 == size)
    {
        return (-1);
    }

    if (name[0] != '/')
    {
        ngx_memcpy(fullname, (const char*)ngx_cycle->prefix.data, ngx_cycle->prefix.len);
        ngx_memcpy(fullname + ngx_cycle->prefix.len, name, size + 1);
    }
    else
    {
        ngx_memcpy(fullname, name, size + 1);
    }

    if (0 != create_logex_directory((char*)fullname))
    {
        return (-1);
    }

    size_t len = strlen((const char*)fullname);
    t->origal_name.data = (u_char*)ngx_pcalloc(cf->pool, len + 1);
    ngx_memcpy(t->origal_name.data, fullname, len);
    t->origal_name.len = len;
    t->level = level;
    t->last_hour = -1;
    t->log_fd = -1;
    t->log_fd_wf = -1;

    return 0;
}

static int32_t create_logex_fd(ngx_lua_logex_t* t)
{
    ngx_tm_t tm;
    get_logex_time(&tm);
    if (t->last_hour != tm.tm_hour)
    {
        t->last_hour = tm.tm_hour;

        u_char curr_name[2048] = { 0 };
        snprintf((char*)curr_name, sizeof(curr_name), "%s.%.4d%.2d%.2d%.2d",
            (char*)t->origal_name.data, tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour);
        if (t->log_fd >= 0)
        {
            close(t->log_fd);
        }
        //must use O_APPEND attribute
        t->log_fd = open((const char*)curr_name, O_CREAT | O_WRONLY | O_APPEND, NGX_FILE_DEFAULT_ACCESS);

        u_char curr_name_wf[2048] = { 0 };
        snprintf((char*)curr_name_wf, sizeof(curr_name_wf), "%s.wf.%.4d%.2d%.2d%.2d",
            (char*)t->origal_name.data, tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour);
        if (t->log_fd_wf >= 0)
        {
            close(t->log_fd_wf);
        }
        //must use O_APPEND attribute
        t->log_fd_wf = open((const char*)curr_name_wf, O_CREAT | O_WRONLY | O_APPEND, NGX_FILE_DEFAULT_ACCESS);
    }

    return 0;
}

int32_t ngx_lua_logex_write(uint32_t level, uint32_t logid, const char* filename, uint32_t line, const char* fmt, ngx_lua_logex_t* t)
{
    u_char logbuffer[4096] = { 0 };
    uint32_t size = 0;
    ngx_tm_t tm;

    if (NULL == filename || NULL == fmt || NULL == t)
    {
        return 0;
    }
    if (level < t->level)
    {
        return 0;
    }

    create_logex_fd(t);

    if (level > COMLOGEX_FATAL)
    {
        level = COMLOGEX_FATAL;
    }

    get_logex_time(&tm);
    size = snprintf((char*)logbuffer, sizeof(logbuffer),
        "%s %u %.4d%.2d%.2d %.2d:%.2d:%.2d %s:%u %s",
        s_log_level[level], logid,
        tm.tm_year, tm.tm_mon, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec, filename, line, fmt);
    if (size >= sizeof(logbuffer))
    {
        size = sizeof(logbuffer) - 1;
    }
    if (size > 0)
    {
        if (logbuffer[size - 1] != '\n')
        {
            logbuffer[size] = '\n';
            size++;
        }

        if (level >= COMLOGEX_WARNING)
        {
            return write(t->log_fd_wf, logbuffer, size);
        }
        else
        {
            return write(t->log_fd, logbuffer, size);
        }
    }
    return 0;
}

/* vi:set ft=c ts=4 sw=4 et fdm=marker: */
