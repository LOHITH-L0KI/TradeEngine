// TradeEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Pool.h"
#include "LFC_Queue.h"
#include "Thread_Util.h"
#include "Logger.h"
#include "MemoryEngine\Heap.h"

using namespace Util;
using namespace Logger;
using namespace Mem;

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
    
    //----------------------------------------------------------------------------
    /*                              MEMORY MANAGER                              */
    //----------------------------------------------------------------------------
    Heap* heap = new Heap(Heap::format::dynamic_blocks, 1024, Heap::align::byte4);
    Heap::Info info;
    
    heap->status(info);
    std::cout << "-------------Heap Status------------\n";
    std::cout << "ToatalSize - " << info.totalSize << std::endl;
    std::cout << "UsedSize - " << info.usedSize << std::endl;
    std::cout << "FreeSize - " << info.freeSize << std::endl;
    std::cout << "CurrUsedBlocks - " << info.currUsedBlocks << std::endl;
    std::cout << "MaxUsedBlocks - " << info.maxUsedBlocks << std::endl;
    std::cout << "ALignment - " << info.heapAlign << "bytes" << std::endl;
    std::cout << "-------------------------------------\n\n";

    void* A = heap->allocate(12);
    heap->status(info);
    std::cout << "-------------Heap Status------------\n";
    std::cout << "ToatalSize - " << info.totalSize << std::endl;
    std::cout << "UsedSize - " << info.usedSize << std::endl;
    std::cout << "FreeSize - " << info.freeSize << std::endl;
    std::cout << "CurrUsedBlocks - " << info.currUsedBlocks << std::endl;
    std::cout << "MaxUsedBlocks - " << info.maxUsedBlocks << std::endl;
    std::cout << "ALignment - " << info.heapAlign << "bytes" << std::endl;
    std::cout << "-------------------------------------\n\n";

    void* B = heap->allocate(7);
    heap->status(info);
    std::cout << "-------------Heap Status------------\n";
    std::cout << "ToatalSize - " << info.totalSize << std::endl;
    std::cout << "UsedSize - " << info.usedSize << std::endl;
    std::cout << "FreeSize - " << info.freeSize << std::endl;
    std::cout << "CurrUsedBlocks - " << info.currUsedBlocks << std::endl;
    std::cout << "MaxUsedBlocks - " << info.maxUsedBlocks << std::endl;
    std::cout << "ALignment - " << info.heapAlign << "bytes" << std::endl;
    std::cout << "-------------------------------------\n\n";

    delete heap;

    //----------------------------------------------------------------------------
    /*                              POOL                                        */
    //----------------------------------------------------------------------------
    Pool<Test_I, 100> *tPool = new Pool<Test_I, 100>();
    std::vector<Test_I*> t(20, nullptr);

    for (int i = 0; i < 20; ++i) {
        t[i] = tPool->allocate(i);

        std::cout << "Address:: " << t[i] << "\n";
        Log::Debug("Address:: {0}", (void*)t[i]);
    }

   //----------------------------------------------------------------------------
   /*                              LFC_QUEUE                                   */
   //----------------------------------------------------------------------------
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
        for(int j = 0; j < 8; ++j)
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
        int j = i;
        wTs[i] = Thread::createAndStartThread(1, "W_" + std::to_string(i), writer, std::move(j));
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
