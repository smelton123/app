#include <assert.h>
#include <string.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <limits.h>
#include <dirent.h>
#include "vkSingleApp.h"

SingleApp *SingleApp::m_self = nullptr;
 
static const char *const kSingleAppDefaultFileName = "/tmp/.mysingleapp";

SingleApp::SingleApp(void)
{
    struct flock lock;
    assert(m_self==nullptr);
    m_self = this;
    m_flag1 = APP_OFFLINE;
    m_flag2 = checkProcess();
    m_fd = open(kSingleAppDefaultFileName, O_WRONLY | O_CREAT, 0666);  
    if (m_fd < 0) 
    {  
        fprintf(stderr,"Fail to open '%s':%s\n", kSingleAppDefaultFileName, strerror(errno));
        m_fd=0;
    } 
    else
    {
        bzero(&lock, sizeof(lock));  

        if (fcntl(m_fd, F_GETLK, &lock) < 0) {  
            fprintf(stderr,"Fail to getlk '%s':%s\n", kSingleAppDefaultFileName, strerror(errno));
        }  

        lock.l_type = F_WRLCK;  
        lock.l_whence = SEEK_SET;  

        if (fcntl(m_fd, F_SETLK, &lock) < 0) {  
            m_flag1 = APP_ONLINE;
        }
    }
}

SingleApp::~SingleApp(void)
{
    close(m_fd);
}

void SingleApp::createInst(void)
{
    if(!m_self)
       m_self =  new SingleApp();
}

void SingleApp::releaseInst(void)
{
    if(m_self){
        delete m_self;
        m_self = nullptr;
    }
}

// @return true: app is online; false app is offline.
SingleApp::APP_STATUS SingleApp::checkProcess(void)
{
    long pid;
    char full_name[PATH_MAX] = {0};
    char binary_path[PATH_MAX] = {0};
    DIR *dir;
    struct dirent * result;

    pid = getpid();

    if (access("/proc/self/exe", F_OK) == 0) {
        int cnt = readlink("/proc/self/exe", binary_path, PATH_MAX);
        if(cnt<0||cnt>PATH_MAX){
            return APP_OFFLINE;
        }    
    }

    dir = opendir ("/proc");
    while ((result = readdir (dir)) != NULL) {
        if (! strcmp(result->d_name, ".") 
                || ! strcmp (result->d_name, "..") 
                || ! strcmp (result->d_name, "thread-self")
                || ! strcmp (result->d_name, "self") 
                || atol (result->d_name) == pid){
            continue;
        }
        memset(full_name, 0, sizeof(full_name));
        sprintf(full_name, "/proc/%s/exe", result->d_name);
        //cout << full_name <<endl;
        if (access(full_name, F_OK) == 0) {
            char path[PATH_MAX]={0};
            int cnt = readlink(full_name, path, PATH_MAX);
            if(cnt<0||cnt>=PATH_MAX) {
                continue ;
            }
            //cout<<"exe:"<<path<<endl;
            if(strcmp(path, binary_path)==0){
                return APP_ONLINE;
            }
        }
    }

    return APP_OFFLINE;
}

bool SingleApp::isRunning(void)
{
    return (m_flag2==APP_ONLINE);
}


















