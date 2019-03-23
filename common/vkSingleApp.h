#ifndef __VKSINGLEAPP_H__
#define __VKSINGLEAPP_H__

class SingleApp final
{
public:
    static inline SingleApp *i(void){ if (!m_self) { defaultInit(); } return m_self; };

    static void createInst(void);
    static void releaseInst(void);
    bool isRunning(void);

public:    
    enum APP_STATUS
    {
        APP_OFFLINE=0,
        APP_ONLINE
    };
private:
    SingleApp(void);
    ~SingleApp(void);
    static inline void defaultInit() {  m_self = new SingleApp();}
    SingleApp & operator = (const SingleApp &); // do not add it's defination.
    APP_STATUS checkProcess(void);

private:
    static SingleApp *m_self;
    int m_fd;
    APP_STATUS m_flag1;
    APP_STATUS m_flag2;
};

#endif
