#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <uv.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "app.h"


App::App(void)
{
    //m_controller = new Controller(process);
    //if (m_controller->init() != 0) {
    //    return;
    //}

    if (!isBackground()) {
        m_console = new Console(this);
    }
}

void App::gotoBackground(void)
{
    signal(SIGPIPE, SIG_IGN);

    if (!isBackground()) {
        return;
    }

    int i = fork();
    if (i < 0) {
        exit(1);
    }

    if (i > 0) {
        exit(0);
    }

    i = setsid();

    if (i < 0) {
        //LOG_ERR("setsid() failed (errno = %d)", errno);
    }

    i = chdir("/");
    if (i < 0) {
        //LOG_ERR("chdir() failed (errno = %d)", errno);
    }
}


void App::onConsoleCommand(char command)
{
    switch (command) {
#   ifdef APP_DEVEL
    case 's':
    case 'S':
        //m_controller->proxy()->printState();
        break;
#   endif

    case 'v':
    case 'V':
        //m_controller->config()->toggleVerbose();
        //LOG_NOTICE("verbose: %d", m_controller->config()->isVerbose());
        break;

    case 'h':
    case 'H':
        //m_controller->proxy()->printHashrate();
        break;

    case 'c':
    case 'C':
        //m_controller->proxy()->printConnections();
        break;

    case 'd':
    case 'D':
        //m_controller->proxy()->toggleDebug();
        break;

    case 'w':
    case 'W':
        //m_controller->proxy()->printWorkers();
        break;

    case 3:
        //LOG_WARN("Ctrl+C received, exiting");
        close();
        break;

    default:
        break;
    }
}    

int App::exec(void)
{
    m_signals = new Signals(this);
    gotoBackground();
    const int r = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    uv_loop_close(uv_default_loop());
    return r;
}


void App::close(void)
{
    uv_stop(uv_default_loop());
}

App::~App(void)
{
    uv_tty_reset_mode();
    delete m_console;
    delete m_signals;
}


void App::onSignal(int signum)
{
    switch (signum)
    {
    case SIGHUP:
        //LOG_WARN("SIGHUP received, exiting");
        break;

    case SIGTERM:
        //LOG_WARN("SIGTERM received, exiting");
        break;

    case SIGINT:
        //LOG_WARN("SIGINT received, exiting");
        break;

    default:
        return;
    }

    close();
}



#if 0
static uv_loop_t* loop = NULL;
static uv_tcp_t server;               // 监听句柄

/*
    数据结构对比

    uv_handle_t 数据结构
    UV_HANDLE_FIELDS

    uv_stream_t 数据结构
    UV_HANDLE_FIELDS
    UV_STREAM_FIELDS

    uv_tcp_t 数据结构
    UV_HANDLE_FIELDS
    UV_STREAM_FIELDS
    UV_TCP_PRIVATE_FIELDS

    uv_tcp_t is uv_stream_t is uv_handle_t
*/

static void on_close(uv_handle_t* handle)
 {
    printf("Close Client\n");
    if (handle->data) {

        free(handle->data);
        handle->data = NULL;
    }
}

static void on_shutdown(uv_shutdown_t* req, int status) 
{
    uv_close((uv_handle_t*)req->handle, on_close);
    free(req);
}

static void after_write(uv_write_t* req, int status) 
{
    if (status == 0) 
    {
        printf("Write sucess\n");
    }

    uv_write_t* w_req = (uv_write_t*)req->data;

    if (w_req) {
        free(w_req);
    }

    free(req);
}

// 当event loop 读完内存时 进行回掉
// strean：发生事件的handle = uv_tcp_t
// nread：读到了多少字节
// buf：数据读到了那个buf里面 buf->base
static void after_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    // 连接断开
    if (nread < 0) {

        uv_shutdown_t* reg = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
        memset(reg, 0, sizeof(uv_shutdown_t));

        uv_shutdown(reg, stream, on_shutdown);
        return;
    }

    buf->base[nread] = 0;
    printf("recv %ld bits\n", nread);
    printf("%s\n", buf->base);

    uv_write_t* w_req = (uv_write_t*)malloc(sizeof(uv_write_t));
    uv_buf_t* w_buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));

    w_buf->base = buf->base;
    w_buf->len = nread;
    // 将buf带入到req中 方便在callback中 free此内存
    w_req->data = w_buf;
    uv_write(w_req, stream, w_buf, 1, after_write);
}

// 当event loop 检测到 HANDLE 上有数据可以读时 进行回掉
// 函数给event loop 准备好读入数据的内存
// handle：发生读事件的handle
// suggested_size：建议我们分配多大的内存
// buf：我们准备好的内存通过buf 告诉event loop
static void uv_alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) 
{
    if (handle->data != NULL) {
        free(handle->data);
        handle->data = NULL;
    }

    buf->base = (char*)malloc(suggested_size + 1);
    buf->len = suggested_size;

    handle->data = buf->base;
}

// uv_listen callback
static void on_connection(uv_stream_t* server, int statu)
{
    printf("New client coming\n");

    uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    memset(client, 0, sizeof(uv_tcp_t));

    uv_tcp_init(loop, client);
    uv_accept(server, (uv_stream_t*)client);

    // 通知event loop 进行管理
    uv_read_start((uv_stream_t*)client, uv_alloc_buf, after_read);
}


int main(int argc, char** argv) 
{
    int ret;
    struct sockaddr_in addr;
    loop = uv_default_loop();
    // Tcp 监听服务
    uv_tcp_init(loop, &server);       // 将server监听句柄加入到event loop
    // 配置event loop 管理类型
    uv_tcp_nodelay((uv_tcp_t*)&server, 1);

    uv_ip4_addr("127.0.0.1", 6080, &addr);

    if (uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0)!=0)
    {
        return -1;
    }

    // 1. event loop 监听管理
    //    当有人连接时 event loop 就会调用uv_connection
    uv_listen((uv_stream_t*)&server, SOMAXCONN, on_connection);

    return uv_run(loop, UV_RUN_DEFAULT);
}
#else

#endif

#if 0 //server 
#define DEFAULT_BACKLOG 10
#define NUM 1000/*1013*/

uv_loop_t *loop;  
struct sockaddr_in addr[NUM];  
struct sockaddr_in addrCli;  
uv_tcp_t server[NUM];  

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {  
    buf->base = (char*) malloc(suggested_size);  
    buf->len = suggested_size;  
}  
  
void echo_write(uv_write_t *req, int status) {  
    if (status) {  
        fprintf(stderr, "Write error %s\n", uv_strerror(status));  
    }  
    free(req);  
}  
  
void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{    
    if (nread < 0) 
    {  
        if (nread != UV_EOF)  
            printf("Read error %ld\n",nread);  
        uv_close((uv_handle_t*) client, NULL);  
        return;
    } 
    else if (nread > 0) 
    {  
        /*uv_write_t *req = (uv_write_t *) malloc(sizeof(uv_write_t)); 
        uv_buf_t wrbuf = uv_buf_init(buf->base, nread); 
        uv_write(req, client, &wrbuf, 1, echo_write);*/  
    }  

    struct sockaddr addrlo1;
    struct sockaddr_in addrin1;  
    int addrlen1 = sizeof(addrlo1);
    char sockname1[17] = {0};

    struct sockaddr addrpeer1;
    struct sockaddr_in addrinpeer1;    
    int addrlenpeer1 = sizeof(addrpeer1);
    char socknamepeer1[17] = {0};

    int ret1 = uv_tcp_getsockname((const uv_tcp_t *)client,&addrlo1,&addrlen1);
    int ret2 = uv_tcp_getpeername((const uv_tcp_t *)client,&addrpeer1,&addrlenpeer1);
    if(0 ==  ret1 && 0 == ret2)
    {
        addrin1 = *((struct sockaddr_in*)&addrlo1);
        addrinpeer1 = *((struct sockaddr_in*)&addrpeer1);
        uv_ip4_name(&addrin1,sockname1,addrlen1);
        uv_ip4_name(&addrinpeer1,socknamepeer1,addrlenpeer1);
        printf("re %s:%d From %s:%d \n",sockname1, ntohs(addrin1.sin_port)/*,buf->base*/,socknamepeer1, ntohs(addrinpeer1.sin_port));  
    }
    else 
        printf("get socket fail %d %d\n",ret1,ret2);
    
    /*buf->base 需要处理，会影响打印*/
    if (buf->base)  
        free(buf->base);  
    
    uv_close((uv_handle_t*) client, NULL); 
}  
  
void on_new_connection(uv_stream_t *server, int status) {  
    if (status < 0) 
    {  
        printf("New connection error %s\n", uv_strerror(status));  
        return;  
    }  
      
    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));  
    uv_tcp_init(loop, client);  

    //client->data = server;暂不需要
    
    if (uv_accept(server, (uv_stream_t*) client) == 0) {  
        uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);  
       // uv_close((uv_handle_t*) client, NULL); 关闭后读不到数据
    }  
    else {  
        uv_close((uv_handle_t*) client, NULL);  
    }  
}  

void creaddr()
{
    int i;
    for(i = 0;i<NUM;i++)
    {
        uv_ip4_addr("127.0.0.1",6000+i,&addr[i]);
        uv_tcp_init(loop, &server[i]);  
        uv_tcp_bind(&server[i], (const struct sockaddr*)&addr[i], 0);  

        if (uv_listen((uv_stream_t*)&server[i], DEFAULT_BACKLOG, on_new_connection)) 
        {  
            printf("Listen error %d\n",i);  //最大支持1014
            return;  
        }  
    };
}

int main() 
{  
    loop = uv_default_loop();  

    creaddr();

    return uv_run(loop, UV_RUN_DEFAULT);
} 
#endif


#if 0
// *************************************************************************
// libuv tcp service code.
#if 1
#define APP_LOGF()                 do{printf("[uvapp@f@%04d]-@%s(): entry!\n", __LINE__, __FUNCTION__);}while(0)
#define APP_LOGD(fmt, args...)     do{printf("[uvapp@d@%04d]-@%s(): " fmt "\n", __LINE__, __FUNCTION__, ##args);}while(0)
#define APP_LOGE(fmt, args...)     do{printf("[uvapp@e@%04d]-@%s(): " fmt "\n", __LINE__, __FUNCTION__, ##args);}while(0)
#else
#define APP_LOGF()                 do {} while (0)
#define APP_LOGD(fmt, args...)     do {} while (0)
#define APP_LOGE(fmt, args...)     do{printf("[uvapp@e@%04d]-@%s(): " fmt "\n", __LINE__, __FUNCTION__, ##args);}while(0)
#endif


static uv_loop_t* loop = NULL;
static uv_tcp_t server;               // 监听句柄


static void on_close(uv_handle_t* handle)
{ 
    APP_LOGF();
    if (handle->data) {

        free(handle->data);
        handle->data = NULL;
    }
}

static void on_shutdown(uv_shutdown_t* req, int status) 
{
    APP_LOGF();
    uv_close((uv_handle_t*)req->handle, on_close);
    free(req);
}

static void after_write(uv_write_t* req, int status) 
{
    APP_LOGF();
    if (status == 0) 
    {
        printf("Write sucess\n");
    }

    uv_write_t* w_req = (uv_write_t*)req->data;

    if (w_req) {
        free(w_req);
    }

    free(req);
}

// 当event loop 读完内存时 进行回掉
// strean：发生事件的handle = uv_tcp_t
// nread：读到了多少字节
// buf：数据读到了那个buf里面 buf->base
static void after_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    APP_LOGF();
    // 连接断开
    if (nread < 0) {

        uv_shutdown_t* reg = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
        memset(reg, 0, sizeof(uv_shutdown_t));

        uv_shutdown(reg, stream, on_shutdown);
        return;
    }

    buf->base[nread] = 0;
    printf("recv %ld bits\n", nread);
    printf("%s\n", buf->base);

    uv_write_t* w_req = (uv_write_t*)malloc(sizeof(uv_write_t));
    uv_buf_t* w_buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));

    w_buf->base = buf->base;
    w_buf->len = nread;
    // 将buf带入到req中 方便在callback中 free此内存
    w_req->data = w_buf;
    uv_write(w_req, stream, w_buf, 1, after_write);
}

// 当event loop 检测到 HANDLE 上有数据可以读时 进行回掉
// 函数给event loop 准备好读入数据的内存
// handle：发生读事件的handle
// suggested_size：建议我们分配多大的内存
// buf：我们准备好的内存通过buf 告诉event loop
static void uv_alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) 
{
    APP_LOGF();
    if (handle->data != NULL) {
        free(handle->data);
        handle->data = NULL;
    }

    buf->base = (char*)malloc(suggested_size + 1);
    buf->len = suggested_size;

    handle->data = buf->base;
}

// uv_listen callback
static void on_connection(uv_stream_t* server, int statu)
{
    APP_LOGD("New client is coming\n");

    uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    memset(client, 0, sizeof(uv_tcp_t));

    uv_tcp_init(loop, client);
    uv_accept(server, (uv_stream_t*)client);

    // 通知event loop 进行管理
    uv_read_start((uv_stream_t*)client, uv_alloc_buf, after_read);
}


int main(int argc, char** argv) 
{
    int ret;
    struct sockaddr_in addr;
    APP_LOGF();
    loop = uv_default_loop();
    // Tcp 监听服务
    uv_tcp_init(loop, &server);       // 将server监听句柄加入到event loop
    // 配置event loop 管理类型
    uv_tcp_nodelay((uv_tcp_t*)&server, 1);

    uv_ip4_addr("127.0.0.1", 6080, &addr);

    if (uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0)!=0)
    {
        APP_LOGE("uv_tcp_bind failed!");
        return -1;
    }        
    

    // 1. event loop 监听管理
    //    当有人连接时 event loop 就会调用uv_connection
    uv_listen((uv_stream_t*)&server, SOMAXCONN, on_connection);

    return uv_run(loop, UV_RUN_DEFAULT);
}
// *************************************************************************
#else
//--------------------------------------------------------------------------
//uv client 
#if 0
#if 1
#define APP_LOGF()                 do{printf("[uvapp@f@%04d]-@%s(): entry!\n", __LINE__, __FUNCTION__);}while(0)
#define APP_LOGD(fmt, args...)     do{printf("[uvapp@d@%04d]-@%s(): " fmt "\n", __LINE__, __FUNCTION__, ##args);}while(0)
#define APP_LOGE(fmt, args...)     do{printf("[uvapp@e@%04d]-@%s(): " fmt "\n", __LINE__, __FUNCTION__, ##args);}while(0)
#else
#define APP_LOGF()                 do {} while (0)
#define APP_LOGD(fmt, args...)     do {} while (0)
#define APP_LOGE(fmt, args...)     do{printf("[uvapp@e@%04d]-@%s(): " fmt "\n", __LINE__, __FUNCTION__, ##args);}while(0)
#endif

uv_loop_t *loop;
#define DEFAULT_PORT 7000
 
uv_tcp_t mysocket;
 
char *path = NULL;
uv_buf_t iov;
char buffer[128];
 
uv_fs_t read_req;
uv_fs_t open_req;
void on_read(uv_fs_t *req);
void on_write(uv_write_t* req, int status)
{
    if (status < 0) 
    {
        fprintf(stderr, "Write error: %s\n", uv_strerror(status));
        uv_fs_t close_req;
        uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
        uv_close((uv_handle_t *)&mysocket,NULL);
        exit(-1);
    }
    else 
    {
        uv_fs_read(uv_default_loop(), &read_req, open_req.result, &iov, 1, -1, on_read);
    }
}
 
void on_read(uv_fs_t *req)
{
    if (req->result < 0) 
    {
        fprintf(stderr, "Read error: %s\n", uv_strerror(req->result));
    }
    else if (req->result == 0) 
    {
        uv_fs_t close_req;
        // synchronous
        uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
        uv_close((uv_handle_t *)&mysocket,NULL);
    }
    else
    {
        iov.len = req->result;
        uv_write((uv_write_t *)req,(uv_stream_t *)&mysocket,&iov,1,on_write);
    }
}
 
void on_open(uv_fs_t *req)
{
    if (req->result >= 0) 
    {
        iov = uv_buf_init(buffer, sizeof(buffer));
        uv_fs_read(uv_default_loop(), &read_req, req->result,&iov, 1, -1, on_read);
    }
    else 
    {
        fprintf(stderr, "error opening file: %s\n", uv_strerror((int)req->result));
        uv_close((uv_handle_t *)&mysocket,NULL);
        exit(-1);
    }
}
 
void on_connect(uv_connect_t* req, int status)
{
    if(status < 0)
    {
        APP_LOGE("Connection error: %s\n",uv_strerror(status));
        return;
    }
 
    fprintf(stdout,"Connect ok\n");
 
    uv_fs_open(loop,&open_req,path,O_RDONLY,-1,on_open);
}
 
int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"Invaild argument!\n");
        exit(1);
    }

    loop = uv_default_loop();
 
    path = argv[1];
 
    uv_tcp_init(loop,&mysocket);
 
    struct sockaddr_in addr;
 
    uv_connect_t* connect = (uv_connect_t*)malloc(sizeof(uv_connect_t));
 
 
    uv_ip4_addr("192.168.8.56",DEFAULT_PORT,&addr);
 
    int r = uv_tcp_connect(connect,&mysocket,(const struct sockaddr *)&addr,on_connect);
 
    if(r)
    {
        APP_LOGE("connect error: %s\n", uv_strerror(r));
        return 1;
    }
 
    return uv_run(loop,UV_RUN_DEFAULT);
}
#endif
//--------------------------------------------------------------------------
#endif



