# RFC1321
md5 implementatio from https://www.ietf.org/rfc/rfc1321.txt

files can be compiled with

gcc -m64 -D MD=5 mddriver.c md5c.c  -o mymd5

this creates an executable file mymd5. Now one can call mymd5. Here the description of this executable form RCF1321:

----------

Arguments (may be any combination):

  -sstring - digests string
  
  -t       - runs time trial
  
  -x       - runs test script
  
  filename - digests file
  
  (none)   - digests standard input



   The MD5 test suite (driver option "-x") should print the following
   results:

MD5 test suite:

MD5 ("") = d41d8cd98f00b204e9800998ecf8427e

MD5 ("a") = 0cc175b9c0f1b6a831c399e269772661

MD5 ("abc") = 900150983cd24fb0d6963f7d28e17f72

MD5 ("message digest") = f96b697d7cb7938d525a2f31aaf161d0

MD5 ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b

MD5 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") =d174ab98d277d9f5a5611c2c9f419d9f

MD5 ("123456789012345678901234567890123456789012345678901234567890123456

78901234567890") = 57edf4a22be3c955ac49da2e2107b67a

----------

But my program has a different output when called with the - option. So I have to check if I made errors when I created the files. Especially when I removed the page headers and footers.
