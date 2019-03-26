#include "app.h"

#if 1
int main(int argc, char **argv)
{
    App app;
    return app.exec();
}
#endif

#if 0
#include <stdio.h>
#include <uv.h>

uv_loop_t *loop;
uv_process_t child_req;
uv_process_options_t options;

void on_exit(uv_process_t *req, int64_t exit_status, int term_signal) {
    fprintf(stdout, "Process exited with status %ld, signal %d.\n", exit_status, term_signal);
    uv_close((uv_handle_t*) req, NULL);
}
void onTimer(uv_timer_t *handle)
{
    printf("timer run\n");
}

int main()
{
    char* args[3];
    args[0] = "./stak";
    args[1] = nullptr;

    options.exit_cb = on_exit;
    options.file = "./stak";
    options.args = args;
    options.flags = UV_PROCESS_DETACHED;
    uv_timer_t *m_timer = new uv_timer_t;

    uv_timer_init(uv_default_loop(), m_timer);   
    uv_timer_start(m_timer, onTimer, 500, 2000);

    int r;
    if ((r = uv_spawn(uv_default_loop(), &child_req, &options))) {
        fprintf(stderr, "%s\n", uv_strerror(r));
        return 1;
    }
    fprintf(stderr, "Launched sleep with PID %d\n", child_req.pid);
    //uv_unref((uv_handle_t*) &child_req);
    //uv_process_kill(&child_req, SIGINT);

    return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

#endif
#if 0
//
// Created by chenchukun on 18/1/20.
//
#include <uv.h>
#include <iostream>
#include <cstring>
using namespace std;

void exit_cb(uv_process_t *handle, int64_t exit_status, int term_signal)
{
    cout << "child process exit, exit_status = " << exit_status
         << ", term_signal = " << term_signal << endl;
    uv_close((uv_handle_t*)handle, NULL);
}

int main()
{
    uv_loop_t *loop = uv_default_loop();

    uv_process_t process;
    uv_process_options_t options = {0};
    options.exit_cb = exit_cb;
    options.file = "ls";
    char* args[3] = {"ls", "-l", NULL};
    options.args = args;
//    options.flags = UV_PROCESS_DETACHED;  // 创建的子进程为守护进程
    // 工作目录
    options.cwd = "/";
    // 环境变量,NULL则使用父进程的环境变量
    options.env = NULL;

    int ret = uv_spawn(loop, &process, &options);
    if (ret != 0) {
        cerr << "uv_spawn error: " << uv_strerror(ret) << endl;
    }
    cout << "child process id = " << process.pid << endl;
    // event-loop会一直监视子进程,调用uv_unref解除对uv_process_t的引用则可以不监视
//    uv_unref((uv_handle_t*)&process);

    // 给子进程发送信号
    uv_process_kill(&process, SIGINT);
    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    return 0;
}
#endif
