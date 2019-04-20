#include "app.h"
#include <unistd.h>
#include <cstring>
#include "vkSummary.h"
#include "common/vkProcess.h"


int main(int argc, char **argv)
{
    App app;
    
    Summary::printVersions();

    return app.exec();
}


