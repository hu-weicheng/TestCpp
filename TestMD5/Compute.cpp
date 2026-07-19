// 微软已启用AMP改用openMP

#include <omp.h>
#include "test.h"

//OpenMP​ CPU多核 <omp.h> #pragma omp ✅ CPU并行

void VectorAdd(float* a, float* b, float* c, int length)
{
    #pragma omp parallel for
    for (int i = 0; i < length; i++)
    {
        c[i] = a[i] + b[i];
    }
}

void test_VectorAdd()
{
    float a[] = { 3, 8, 9, 4 }, b[] = { 9,11,4,5 };
    float c[sizeof(a)];
    VectorAdd(a, b, c, 4);
    for (int i = 0; i < 4; i++)
    {
        cout << c[i] << " = " <<a[i] << " + " <<b[i]<< endl;
    }
}



