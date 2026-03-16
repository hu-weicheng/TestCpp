// TestStructOverLoadFun.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <cmath>
#include <string>

using namespace std;

struct complexFreq
{
    int r;  //电阻
    int w;  //角频率
    // 构造函数
    //complexFreq(int resistance, int omega) : r(resistance), w(omega) {}
private:
    int inline mod();    //模
public:
    void print();
    void print(const string format);
};

int complexFreq::mod()
{
    return sqrt(pow(r, 2) * pow(w, 2));
}

void complexFreq::print()
{
    cout << "Z=" << r << " + j" << w << endl;
}

void complexFreq::print(const string format)
{
    if (format == string("all"))
    {
        cout << "Z=" << r << " + j" << w << endl;
        cout << "|Z|=" << mod() << endl;
    }
}

int main()
{
    complexFreq z1 = complexFreq();
    //complexFreq z2(3, 2);
    //z2.print();
    z1.print();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
