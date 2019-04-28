#include "vkMd5sum.h"
#include <stdio.h>
#include <stdlib.h>


std::string Md5sum::HexDigest() const
{
    char buf[33];
    for (int i=0; i<16; i++)
        sprintf(buf+i*2, "%02x", m_digest[i]);
    buf[32]=0;
    return std::string(buf);
}

int Md5sum::Md5File(const char *filename)
{
    FILE *file;
    MD5_CTX context;
    int len;
    unsigned char buffer[1024];

    if (!filename) 
        return 0;

    if ((file = fopen (filename, "rb")) == NULL)
    {
        printf ("%s can't be opened\n", filename);
        return -1;
    }
    else 
    {
        MD5Init (&context);
        while ((len = fread(buffer, 1, 1024, file))){
            MD5Update (&context, buffer, len);
        }
        MD5Final (m_digest, &context);
        fclose (file);
    }   
    return 0;
}

void Md5sum::Md5String(const char *string)
{
    MD5_CTX context;
    unsigned int len = strlen (string);

    MD5Init(&context);
    MD5Update(&context, (unsigned char*)string, len);
    MD5Final(m_digest, &context);
}

void Md5sum::Md5Print(void)
{
    int i = 0;
    printf("md5sum:");
    for (i = 0; i < 16; i++)
        printf ("%02x", m_digest[i]);
    printf("\n");
}

