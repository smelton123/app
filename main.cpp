#include "app.h"
#include <unistd.h>
#include <cstring>
#include "vkSummary.h"
#include "common/vkProcess.h"


int main(int argc, char **argv)
{
    // check app version info!
    if ((argc == 2))
    {
        if (strcmp(argv[1],"--info")==0)
            Summary::printVersions();
        
        return 0;       
    }
    else if(argc>2)
    {
        return 0;
    }
    else
    {
        App app;
        return app.exec();
    }
    
    return 0;
}


