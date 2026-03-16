// Study.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 加分题
// Author：柴锦
// 
//

#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <ctime>
using namespace std;

class mMatrix
{
public:
	mMatrix();
	mMatrix(unsigned char rows, unsigned char column);
	void SetData(unsigned char rows, unsigned char column);
	void Generate();
	void Print();
	vector<unsigned char> Find1();
	int Find1_RowNo();
	~mMatrix();
private:
	inline void initBuffer(unsigned char rows, unsigned char column);

private:
	uint16_t rows;
	uint16_t column;
	unsigned char* pBuf;
};

mMatrix::mMatrix(unsigned char rows, unsigned char column)
{
	this->rows = rows;
	this->column = column;
	pBuf = new unsigned char[rows * column];
}

mMatrix::mMatrix()
{
	rows = 3;
	column = 3;
	pBuf = new unsigned char[rows * column];
}

mMatrix::~mMatrix()
{
	if (pBuf)
		delete pBuf;
}

void inline mMatrix::initBuffer(unsigned char rows, unsigned char column)
{
	this->rows = rows;
	this->column = column;
	pBuf = new unsigned char[rows * column];
}

void mMatrix::Generate()
{
	srand(time_t());

	for (int i = 0; i < rows * column; i++)
	{
		pBuf[i] = rand() % 255;
	}
}
void mMatrix::Print()
{
	cout << "This Matrix Data is as Follows:" << endl;
	for (int i = 0; i < rows * column; i++)
	{
		cout << std::setw(3) << std::left<<(int)pBuf[i] << " ";
		if (i % column + 1 == column)
			cout << endl;
	}	
}

//序数从1开始而不是从0开始
//没有返回0
//返回的是第一个满足条件的行号
int mMatrix::Find1_RowNo()
{
	//rows * column /rows	每行的个数
	for (int i = 0; i < column; i++)
	{
		if (pBuf[i] == 1)
		{
			return i/column+1;
		}
		i += column;
	}
	return 0;
}
vector<unsigned char> mMatrix::Find1()
{
	auto tmp = vector<unsigned char>();
	//rows * column /rows	每行的个数
	for (int i = 0; i < column; i++)
	{
		if (pBuf[i] == 1)
		{
			for(int j=i;j<column;j++)
				tmp.push_back(pBuf[j]);
		}
		i += column;
	}
	return tmp;
}

void mMatrix::SetData(unsigned char rows, unsigned char column)
{
	initBuffer(rows, column);
	cout << "Please Input Matrix Data:" << "\n";
	int temp = 0;
	for (int i = 0; i < rows * column; i++)
	{
		cin >> temp;
		pBuf[i]=(unsigned char)temp;	//解决输入的过程中进行了隐式转换
	}
}

int main()
{
	mMatrix mat;
	//mat.Generate();
	mat.SetData(3, 3);
	mat.Print();
	auto result = mat.Find1();
	for (int i = 0; i < result.size(); i++)
	{
		cout << result.at(i) << " ";
	}
	cout << "Fond Fist Data Rows Nunber is "<<mat.Find1_RowNo() << endl;
}
