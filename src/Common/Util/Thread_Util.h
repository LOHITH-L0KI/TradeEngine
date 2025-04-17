#include <string>
#include <atomic>
#include <thread>
#include <iostream>
#include <pthread.h>

#ifndef THREAD_UTIL_H
#define THREAD_UTIL_H

namespace Common {

	template<typename T, typename... A>
	inline auto createAndStartThread(const int coreId, const std::string& name, T &&func, A&&... args) noexcept
	{
		std::atomic_bool failed = false, running = false;

		auto tBody = [&] {
			if (coreId >= 0 && !setThreadCore(coreId)) {
					
				std::cerr << "Failed to set core affinity for "
					<< name << " "
					<< pthread_self() << "to"
					<< coreId << '\n';

					failed = true;
					return;
			}

			std::cout << "Successfully set core affinity for "
				<< name << " "
				<< pthread_self() << "to"
				<< coreId << '\n';

			running = true;
			
			//execute the function
			std::forward<T>(func)((std::forward<A>(args))...);
		}

		std::thread* t = new std::thread(tBoby);

		while (!running && !failed) {
			std::this_thread::sleep_for(1s);
		}

		if (failed) {
			t->join();
			delete t;
			t = nullptr;
		}

		return t;
	}

	//PIN THREAD TO CPU CORE WITH ID = "coreId"
	bool setThreadCore(const int coreId) noexcept {

		cpu_set_t cpuSet;
		CPU_ZERO(&cpuSet);				//SET 0 AS CORE_ID
		CPU_SET(coreId, &cpuSet);		//SET REQUIRED CORE_ID

		return pthread_setaffinity_np(pthread_self(), sizeof(cpuSet), &cpuSet) == 0;
	}

}

#endif // !THREAD_UTIL_H