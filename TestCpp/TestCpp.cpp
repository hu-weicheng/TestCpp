// TestCpp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
using namespace std;

template<typename Tp>
Tp max(Tp a, Tp b)
{
    return a > b ? a : b;
}
template<class T>
T min(T a, T b)
{
    return a < b ? a : b;
}

//template<size_t Tt>
//Tt max(Tt a,int Tt)
//{
//    return a>b?a:b;
//}

int main()
{
    size_t a = 3, b = 5;
    cout <<::max(a, b) << endl;
    cout<<::min(a,b)<<endl;
    return 0;
}

