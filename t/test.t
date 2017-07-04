#user  nobody;
worker_processes  1;

#error_log  logs/error.log;
#error_log  logs/error.log  notice;
#error_log  logs/error.log  info;

#pid        logs/nginx.pid;

events {
    worker_connections  1024;
}

http {
    include       mime.types;
    default_type  application/octet-stream;

    sendfile        on;
    keepalive_timeout  65;
    gzip  on;

    lua_logex_file_path /home/work/xxx/logs/http_ex.log;
    lua_logex_level 3;

    server {
        listen       8681;
        server_name  localhost;

        set $logex_id $logid;
        lua_logex_file_path /home/work/xxx/logs/server_ex.log;
        lua_logex_level 2;

        location /foo {

            lua_logex_file_path /home/work/xxx/logs/location_ex.log;
            lua_logex_level 2;

            default_type text/css;

            content_by_lua '
                local logex = require "ngx.ext"
                --logex.log(level, src_file_name, line, fmt)
                --level value:0-debug, 1-trace, 2-notice, 3-warning, 4-fatal
                logex.log(1, "test.conf", 31, "111" .. ngx.http_time(ngx.time()))
                logex.log(2, "test.conf", 32, "222 " .. ngx.http_time(ngx.time()))
                logex.log(3, "test.conf", 33, "333 " .. ngx.http_time(ngx.time()))
                logex.log(4, "test.conf", 34, "444 " .. ngx.http_time(ngx.time()))
                ';
        }

        location / {
            root   html;
            index  index.html index.htm;
        }

        error_page   500 502 503 504  /50x.html;
        location = /50x.html {
            root   html;
        }
    }
