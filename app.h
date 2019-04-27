#ifndef __APP_H__
#define __APP_H__
#include "common/interfaces/vkIConsoleListener.h"
#include "common/interfaces/vkISignalListener.h"
#include "common/vkConsole.h"
#include "common/vkSignals.h"

class App final: public IConsoleListener, public ISignalListener
{
public:
    App(void);
    ~App(void) override;

    int exec(void);

protected:
    void onConsoleCommand(char command) override;
    void onSignal(int signum) override;

private:
    void gotoBackground(void);
    inline bool isBackground(void) {return 1;}
    void close();

    Console *m_console;
    //Controller *m_controller;
    //Httpd *m_httpd;
    Signals *m_signals;
};

#endif
