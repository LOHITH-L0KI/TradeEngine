// TradeEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Pool.h"
#include "LFC_Queue.h"
#include "Thread_Util.h"
#include "Logger.h"

using namespace Util;
using namespace Logger;

struct Test{

    Test():i(0), f(0.0), b(false){}

    Test(int _i, float _f, bool _b)
        :i(_i), f(_f), b(_b)
    {  }

    int i;
    float f;
    bool b;
};

struct Test_I {
    Test_I() : i(0){}

    Test_I(int _i)
        :i (_i){ }

    int i;
};

int main()
{
    //default config
    TextLoggerConfig db;
    Log::Configure<LogType::TEXT>(RunMode::CONCURRENT_THREAD, db);

    std::cout << "TradeEngine\n";
    Log::Info("TradeEngine");


    //POOL TESTING
    Pool<Test_I, 100> *tPool = new Pool<Test_I, 100>();
    std::vector<Test_I*> t(20, nullptr);

    for (int i = 0; i < 20; ++i) {
        t[i] = tPool->allocate(i);

        std::cout << "Address:: " << t[i] << "\n";
        Log::Debug("Address:: {0}", (void*)t[i]);
    }

    //LFC_Queue TESTING
   const size_t queueSize = 1 << 3;
    LFC_Queue<Test_I, queueSize>* queue = new LFC_Queue<Test_I, queueSize>();
    std::atomic_bool* kill = new std::atomic_bool(false);
    std::atomic_bool* startTrig = new std::atomic_bool(false);

    //reader thread
    auto reader = [&] {

        std::cout << "R:: Ready\n";
        Log::Debug("R:: Ready");

        //wait for start trigger
        while (!startTrig->load(std::memory_order_relaxed));

        Test_I read;
        while (!kill->load(std::memory_order_relaxed) || queue->size() > 0) {

            if (queue->pop(read)) {

                std::cout << "R:: " << read.i << "\n";
                Log::Debug("R:: {0}", read.i);
            }
        }

        std::cout << "R:: Done\n";
    };

    //Reader thread
    std::thread* rT = Thread::createAndStartThread(1, "R", reader);
    std::atomic_int* counter = new std::atomic_int(0);

    auto writer = [&](int index) {
        std::cout << "W_" << index << ":: Ready\n";
        Log::Debug("W_{0}:: Ready", index);

        if (counter->fetch_add(1, std::memory_order_acq_rel) == 19) {
            
            std::cout << "\n----------------------------------\n";
            Log::Info("------------------------------");
            std::cout << "W_" << index << ":: " << "Triggering Threads(" << counter->load() <<  ")Start.\n";
            Log::Debug("W_{0}:: Triggering Thread({1})Start.", index, counter->load());

            startTrig->store(true);
        }

        //wait for start trigger
        while (!startTrig->load(std::memory_order_relaxed));
        
        //wait till the data is written
        while (!queue->push(*(t[index])));

        std::cout << "W_" << index << ":: " << t[index]->i << '\n';
        Log::Debug("W_{0}:: {1}", index, t[index]->i);

        if (counter->fetch_sub(1, std::memory_order_acq_rel) == 1) {
            std::cout << "\n----------------------------------\n";
            Log::Info("------------------------------");

            std::cout << "W_" << index << ":: " << "Kill Trigger\n";
            Log::Debug("W_{0}:: Kill Trigger", index);
            
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
    Log::Destroy();
}
