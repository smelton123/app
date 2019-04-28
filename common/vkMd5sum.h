#ifndef __VKMD5SUM_H__
#define __VKMD5SUM_H__

#include <cstring>
#include <iostream>
#include "../third-party/md5/md5.h"


class Md5sum final
{
public:    
    Md5sum(){ m_digest[0]='\0';};
    void Md5String(const char *string);
    int Md5File(const char *filePath);
    //void Md5Memory(const void* p, unsigned int len);
    std::string HexDigest() const;
    void Md5Print(void);
private:
    unsigned char m_digest[16]; // the result
};

#endif