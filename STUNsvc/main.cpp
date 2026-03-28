


//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#include <winsvc.h>
//#include <stdio.h>
//
//#pragma comment(lib, "advapi32.lib")
//
////简单的服务测试代码
//int main() {
//    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
//    if (scm) {
//        printf("Windows服务API测试成功！\n");
//        CloseServiceHandle(scm);
//    }
//    return 0;
//}

#include <windows.h>
#include <winsvc.h>

#include "svc.h"

#pragma comment(lib, "Advapi32.lib")

int wmain() 
{
    SERVICE_TABLE_ENTRY serviceTable[] = {
        { (LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTIONW)ServiceMain },
        { NULL, NULL }
    };

    // 连接到服务控制管理器并启动控制分派器
    if (!StartServiceCtrlDispatcher(serviceTable)) 
    {
        // 如果调用失败，可能是作为控制台程序运行
        // 这里可以添加普通运行模式的代码
        return GetLastError();
    }
    return 0;
}