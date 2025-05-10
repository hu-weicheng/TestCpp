
#include <stdio.h>
#include <Windows.h>
#include <Wininet.h>

#pragma comment(lib, "Wininet.lib")

#define URL L"http://www.baidu.com/"
#define USER_AGENT L"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET4.0C; .NET4.0E; .NET CLR 2.0.50727)"

bool OpenUrl()
{
	HINTERNET hInternet, hInternetUrl;
	hInternet = InternetOpen(USER_AGENT, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	if (!hInternet) {
		wprintf(L"InternetOpen error: %d\n", GetLastError());
		return false;
	}
	hInternetUrl = InternetOpenUrl(hInternet, URL, NULL, 0, INTERNET_FLAG_HYPERLINK, NULL);
	if (!hInternetUrl) {
		wprintf(L"InternetOpenUrl error: %d\n", GetLastError());
		InternetCloseHandle(hInternet);
		return false;
	}
	InternetCloseHandle(hInternetUrl);
	InternetCloseHandle(hInternet);
	return true;
}



int main()
{
	OpenUrl();
	return 0;
}
