#include "app.h"


int main(int argc, char **argv)
{
    App app;
    return app.exec();
}
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
