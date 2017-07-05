# ngx_lua_logex-module
业务日志nginx module，相比其它相似的日志模块，其优势主要如下：<br>
1）快速写日志；<br>
2）可在nginx conf下配置日志输出路径以及日志级别；<br>
3）自动生成普通日志和告警日志，并分别按小时级进行切割；<br>
4）自带变量logex_id，方便进行问题追踪；

# 模块指令说明：
lua_logex_file_path<br>
语法：lua_logex_file_path <logfile_path><br>
默认值：无<br>
配置段：http, server, location<br>
描述：设置输出日志文件路径，若未设置全路径则输出到nginx路径下，若设置全路径则根据全路径输出，假定设置的日志文件名为lua.log，则最终会按照小时级切割成两种日志，如：lua.log.2017070418和lua.log.wf.2017070418，低于NOTICE（包含NOTICE）日志级别的写入前者，高于NOTICE日志级别的写入后者。

lua_logex_level<br>
语法：lua_logex_level <level><br>
默认值：2<br>
配置段：http, server, location, location if<br>
描述：设置能够输出到日志文件的最低级别，总共有5个级别，分别是：0-DEBUG, 1-TRACE, 2-NOTICE, 3-WARNING, 4-FATAL，缺省为NOTICE。

#模块变量说明：
$logex_id<br>
对于每个请求都会自动生成一个logex_id，便于追踪排查问题。

# lua接口函数说明：
log(level, src_file_name, line, fmt)<br>
功能：写日志到日志文件；
参数1：当前日志的级别，取值范围为0—4；<br>
参数2：产生该条日志的源码文件名；<br>
参数3：产生该日志的源码代码行；<br>
参数4：日志内容；<br>

# 示例：  
    #user  nobody;
    worker_processes 1;

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

        log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
                  '$status $body_bytes_sent "$http_referer" '
                  '"$http_user_agent" "mylogexid:$logex_id"';

        access_log  logs/access.log  main;

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

                lua_logex_file_path logs/location_ex.log;
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
