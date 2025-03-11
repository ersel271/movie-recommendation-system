#ifndef THREADHANDLER_H
#define THREADHANDLER_H

#include <thread>
#include <vector>
#include <mutex>

class ThreadHandler {
public:
	//Constructor.
    ThreadHandler(size_t numThreads = std::thread::hardware_concurrency());
    
    //Run threads.
    void runParallel(const std::function<void(size_t, size_t)>& task, size_t totalWork);

    //Lock and unlock operations (w/mutex).
    void lock();
    void unlock();

private:
    size_t numThreads; 				  //Number of threads.
    std::mutex mutex; 				  //A global mutex variable for locks.
    std::vector<std::thread> threads; //List of threads.
};

#endif // THREADHANDLER_H

