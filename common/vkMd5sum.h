#ifndef __VKMD5SUM_H__
#define __VKMD5SUM_H__

#include <cstring>
#include <iostream>
#include "../third-party/md5/md5.h"


class Md5sum final
{
public:    
    Md5sum(){};
    void md_string(const char *string);
    int md_file(const char *filePath);
    void md_memory(const void* p, unsigned int len);
    std::string hexdigest() const;
    void md_print(void);
private:
    unsigned char m_digest[16]; // the result
};

#endif