// TestCpp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <type_traits>  //C++ 11
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



template<typename T>
constexpr T maxV2(T a, T b)
{
    static_assert(std::is_integral<T>::value, "Integral type required");//限制为整数类型
    return a > b ? a : b;
}

int main()
{
    size_t a = 3, b = 5;
    cout <<::max(a, b) << endl;
    cout<<::min(a,b)<<endl;
    return 0;
}

