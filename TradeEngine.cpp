// TradeEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Pool.h"
#include "LFC_Queue.h"
#include "Thread_Util.h"

using namespace Util;

struct Test{

    Test(){}

    Test(int _i, float _f, bool _b)
        :i(_i), f(_f), b(_b)
    {  }

    int i;
    float f;
    bool b;
};

struct Test_I {
    Test_I(){}

    Test_I(int _i)
        :i (_i){ }

    int i;
};
int main()
{
    std::cout << "TradeEngine\n";

    //POOL TESTING
    Pool<Test_I, 100> *tPool = new Pool<Test_I, 100>();
    std::vector<Test_I*> t(20, nullptr);

    for (int i = 0; i < 20; ++i) {
        t[i] = tPool->allocate(i);

        std::cout << "Address:: " << t[i] << "\n";
    }

    //LFC_Queue TESTING
   const size_t queueSize = 1 << 3;
    LFC_Queue<Test_I, queueSize>* queue = new LFC_Queue<Test_I, queueSize>();
    std::atomic_bool* kill = new std::atomic_bool(false);
    std::atomic_bool* startTrig = new std::atomic_bool(false);

    //reader thread
    auto reader = [&] {

        std::cout << "R:: Ready\n";

        //wait for start trigger
        while (!startTrig->load(std::memory_order_relaxed));

        Test_I read;
        while (!kill->load(std::memory_order_relaxed) || queue->size() > 0) {

            if (queue->pop(read)) {

                std::cout << "R:: " << read.i << "\n";
            }
        }

        std::cout << "R:: Done\n";
    };

    //Reader thread
    std::thread* rT = Thread::createAndStartThread(1, "R", reader);
    std::atomic_int* counter = new std::atomic_int(0);

    auto writer = [&](int index) {
        std::cout << "W_" << index << ":: Ready\n";

        if (counter->fetch_add(1, std::memory_order_acq_rel) == 19) {
            
            std::cout << "\n----------------------------------\n";
            std::cout << "W_" << index << ":: " << "Triggering Threads(" << counter->load() <<  ")Start.\n";
            startTrig->store(true);
        }

        //wait for start trigger
        while (!startTrig->load(std::memory_order_relaxed));
        
        //wait till the data is written
        while (!queue->push(*(t[index])));

        std::cout << "W_" << index << ":: " << t[index]->i << '\n';

        if (counter->fetch_sub(1, std::memory_order_acq_rel) == 1) {
            std::cout << "\n----------------------------------\n";
            std::cout << "W_" << index << ":: " << "Kill Trigger\n";
            
            kill->store(true, std::memory_order_seq_cst);
        }

        tPool->dealloc(t[index]);

        //wait till kill command is triggered
        //std::cout << "W_" << index << ":: Done\n";

        while (!kill->load(std::memory_order_relaxed));
    };


    std::vector<std::thread*> wTs(20, nullptr);

    for (int i = 0; i < 20; i++) {
        wTs[i] = Thread::createAndStartThread(1, "W_" + std::to_string(i), writer, i);
    }

    
    //destroy flow.
    while (!kill->load(std::memory_order_relaxed));
    for (auto t : wTs)
        t->join();

    rT->join();
    delete counter;
    delete startTrig;
    delete kill;
    delete queue;
    delete tPool;
}
