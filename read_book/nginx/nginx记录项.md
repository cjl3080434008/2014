1. 基本的启动、关闭、升级命令

2.配置文件格式(一个配置模块，对应一个解析这个模块的解析程序)
模块名{
	内嵌模块名1{
	
	}
	
	内嵌模块名2{
		配置项名 配置项值1（数字，字符串，regx） 配置项值2 ...;
	}
}

嵌套模块间的命名冲突： 取决于解析这个配置项的模块。

是否以 master/worker方式工作：
master_process on|off;

error日志设置：
error_log /path/file level;
error_log logs/error.log error; ----> 写入error.log日志文件中的数据是 >= level的错误级别数据。


嵌入其他配置文件：
include 相对位置的配置文件 | 绝对位置的配置文件;

pid文件路径:
pid path/file;
pid logs/nginx.pid;

Nginx worker进程运行的用户及用户组：
user username [groupname];
user nobody nobody;
user root root;

限制信号队列：
限制每个用户发往Nginx的信号队列大小--->信号多余队列的大小，则丢弃后来的信号。
worker_rlimit_sigpending limit;

// 优化性能的配置项
进程数  worker_progress number;
指定nginx worker进程到指定的CPU内核：
worker_cpu_affinity cpumask[cpumask...];  (仅对linux操作系统有效)
worker_cpu_affinity 1000 0100 0010 0001;

// 事件配置项
accept_mutex on|off;			是nginx的负载均衡锁，当某个worker线程连接数达到worker_connections配置的最大数的7/8时，会大大减少其再接收新连接的机会。

accept_mutex可能用到的文件锁路径：
lock_file path/file;
lock_file logs/nginx.lock;

worker的最大连接数：
worker_connections nums;


//	基本的http服务器配置
1.监听端口：
	listen address:port[default | default_server | [ backlog=num | rcvbuf=size | sndbuf=size|accept_filter=filter|bind|ssl]];

2.主机名称
server_name name;
用于nginx解析请求的时候根据header头的Host来进行server_name的匹配来确定哪个server块进行处理。

3.location定位符
location [=|~|~*|^~|@] /uri/{...}
'~'		----> 表示字母大小写敏感
'~*'	----> 忽略字母大小写
'^~'	----> 匹配URI的前半部分
location ^~ /images/ {	# 以/images/开始的请求都匹配 }
location ~* \.(gif|jpg|jpeg)${ # 匹配.gif, .jpg, .jpeg结尾的请求	}


4.文件路径定义：
4.1以root方式设置资源路径：
root path;
location /download/{
	root /opt/web/html/;
}
当uri请求为/download/index/test.html的时候，则是返回服务器上$root/download/index/test.html文件的内容。   

4.2访问首页
index file...;
设置多个首页访问，nginx将依次访问该页面，如果可以访问，则立即返回数据。

4.3根据HTTP返回码重定向页面
error_page code[=answer code] uri|@named_location;

location / {
	error_page 404 @fallback;
	// error_page 404 /fallback;
}
location @fallback{
	proxy_pass http://backend;
}

reset_timeout_connection on|off; 连接超时的处理操作开启。如果设置了，当链接超时时Nginx直接发送一个RST包，并释放服务端中的所有连接内存数据，而不通过4次握手断开。


// 2.5用nginx搭建一个反向代理服务器
proxy_pass URL;
upstream



