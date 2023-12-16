#include<openssl/md5.h>

void calculate_md5(char *str, unsigned char result[16]){
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, str, strlen(str));
    MD5_Final(result, &ctx);
}