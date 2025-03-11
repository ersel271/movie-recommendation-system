#include "ThreadHandler.h"

ThreadHandler::ThreadHandler(size_t numThreads) {
    this->numThreads = (numThreads <= 1) ? 2 : std::min(numThreads, static_cast<size_t>(std::thread::hardware_concurrency()));
}

void ThreadHandler::runParallel(const std::function<void(size_t, size_t)>& task, size_t totalWork) {
    size_t chunkSize = totalWork / numThreads;

    for (size_t t = 0; t < numThreads; ++t) {
        size_t start = t * chunkSize;
        size_t end = (t == numThreads - 1) ? totalWork : start + chunkSize;
        threads.emplace_back(task, start, end);
    }
    for (auto& thread : threads) if (thread.joinable()) thread.join();
    threads.clear();
}

void ThreadHandler::lock() {
    mutex.lock();
}

void ThreadHandler::unlock() {
    mutex.unlock();
}

