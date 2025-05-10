#include <stdio.h>  

int main(int argc, char* argv[])
{
    unsigned int a = 0x12345678;
    unsigned char* p = (unsigned char*)&a; // 取低地址上的数据
    if (0x12 == *p) {
        printf("Big-Endian\n");
    }
    else if (0x78 == *p) {
        printf("Little-Endian\n");
    }
    return 0;
}