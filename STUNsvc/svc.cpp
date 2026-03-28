#include <windows.h>
#include <winsvc.h>
#include "svc.h"
#pragma comment(lib, "Advapi32.lib")



// 全局变量声明
SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE hServiceStatusHandle;
BOOL bRunning = TRUE;


// 服务控制处理函数
void WINAPI ServiceHandler(DWORD fdwControl) 
{
    switch (fdwControl) {
    case SERVICE_CONTROL_STOP:
        serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        bRunning = FALSE;
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        bRunning = FALSE;
        break;
    default:
        break;
    }
    SetServiceStatus(hServiceStatusHandle, &serviceStatus);
}

// 服务主函数
void WINAPI ServiceMain(int argc, char** argv) 
{
    // 注册服务控制处理程序
    hServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceHandler);

    if (!hServiceStatusHandle) return;

    // 初始化服务状态
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    serviceStatus.dwWin32ExitCode = 0;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 3000;

    SetServiceStatus(hServiceStatusHandle, &serviceStatus);

    // 服务初始化完成，报告运行状态
    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;
    SetServiceStatus(hServiceStatusHandle, &serviceStatus);

    // 主服务循环 - 在此集成STUNMAN的核心功能
    while (bRunning) {
        // 调用STUNMAN的主要功能函数
        // RunStunmanCoreLogic(); 
        Sleep(1000); // 模拟工作负载
    }

    // 清理资源
    serviceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hServiceStatusHandle, &serviceStatus);
}

BOOL InstallService() 
{
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hSCM) return FALSE;

    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);

    SC_HANDLE hService = CreateService(
        hSCM, SERVICE_NAME, SERVICE_NAME,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
        szPath, NULL, NULL, NULL, NULL, NULL);

    if (!hService) {
        CloseServiceHandle(hSCM);
        return FALSE;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    return TRUE;
}

BOOL UninstallService()
{
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL) return FALSE;

    // 打开已存在的服务[7](@ref)
    SC_HANDLE hService = OpenService(hSCM, SERVICE_NAME, DELETE);
    if (hService == NULL) {
        CloseServiceHandle(hSCM);
        return FALSE;
    }

    BOOL bResult = DeleteService(hService); // 删除服务

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    return bResult;
}

void LogEvent(LPCTSTR pFormat, ...) 
{
    TCHAR chMsg[256];
    HANDLE hEventSource;
    LPTSTR lpszStrings[1];

    hEventSource = RegisterEventSource(NULL, SERVICE_NAME);
    if (hEventSource) {
        lpszStrings[0] = chMsg;
        ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*)&lpszStrings[0], NULL);
        DeregisterEventSource(hEventSource);
    }
}