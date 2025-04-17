// TradeEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Pool.h"

using namespace Common;

struct Test{

    Test(){}

    Test(int _i, float _f, bool _b)
        :i(_i), f(_f), b(_b)
    {  }

    int i;
    float f;
    bool b;
};
int main()
{
    std::cout << "TradeEngine\n";
    Pool<Test, 100> *tPool = new Pool<Test, 100>();
    Test* t[100];
    for (int i = 0; i < 20; ++i) {
        t[i] = tPool->allocate(10, 12.6f, false);

        if (t)
            std::cout << "Address:: " << t << "\n";
    }

    tPool->dealloc(t[5]);

    bool ex;
    std::cin >> ex;
}
