/*************************************************************************************************
*本质是 UTF-8 编码的中文字符被 Windows 控制台（默认 GBK/CP936）按 GBK 强行解码导致的。。
*结合你前面遇到的 C4819 警告，说明源文件编码、编译器解析和控制台输出这三层没有对齐
*************************************************************************************************/

#include "test.h"
#include <Windows.h>

int main() 
{
    SetConsoleOutputCP(CP_UTF8);
    const char* test_str = "hello world";
    cout << "MD5(\"" << test_str << "\") = " ;
    cout<< md5(test_str, strlen(test_str)) << endl;
    //test_VectorAdd();
    testFFT();

    return 0;
}