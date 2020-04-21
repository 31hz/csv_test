#include "workDispatcher.h"

#include <string>
#include <mutex>
#include <glob.h>
#include <thread>
#include <vector>
#include <chrono>


// Do the file glob comparison and return count of found files
// Also update the private variable _fileQueue
int workDispatcher::find_glob_files() {
    int count = 0;

    glob_t glob_res;
    glob(_csvglob.c_str(),GLOB_TILDE,NULL,&glob_res);

    for (unsigned int i=0; i < glob_res.gl_pathc; ++i) {
        _fileQueue.push(std::string(glob_res.gl_pathv[i]));
        count++;
    }

    globfree(&glob_res);
    return count;
}

// Return the count of finished threads
int workDispatcher::get_finished_count() {
    std::lock_guard<std::mutex> lck (_threads_finished_mutex);
    return _threads_finished_count;
}

// Increment the finished thread count.
// Note this is called from the worker thread
int workDispatcher::inc_finished_count() {
    std::lock_guard<std::mutex> lck (_threads_finished_mutex);
    _threads_finished_count++;
    return _threads_finished_count;
}

// Remove one filename from the work queue and return it
// Note this is called from the worker thread
std::string workDispatcher::popOneFile() {
    std::lock_guard<std::mutex> lck (_fileMutex);
    std::string result("");

    if (! _fileQueue.empty()) {
        result = _fileQueue.front();
        _fileQueue.pop();
    }

    return result;
}

// Constructor
workDispatcher::workDispatcher(const int concurrency, const int column, const std::string agg, const std::string csvglob) {
    _concurrency = concurrency;
    _column = column;
    _agg = agg;
    _csvglob = csvglob;
    _threads_finished_count = 0;
}

// Destructor
workDispatcher::~workDispatcher() {
}

// Perform the bulk of the work processing
void workDispatcher::run() {
    int count;
    int finished_count;
    std::vector<std::thread*> all_threads;

    count = find_glob_files();

    if (count > 0) {

        for (unsigned int i = 0; i < _concurrency; i++) {
            all_threads.push_back(new std::thread(worker(), this, _concurrency, _agg));
        }

        // Wait for all threads to be done
        finished_count = get_finished_count();
        while (finished_count < count) {

            // Sleep a little and wait for threads to finish
            // TODO: Ideally this would be event-driven somehow instead of sleeping
            std::this_thread::sleep_for(std::chrono::milliseconds(_WAIT_DELAY_MS));

            finished_count = get_finished_count();
        }

        // Now join all threads
        for (unsigned int i = 0; i < _concurrency; i++) {
            all_threads.at(i)->join();
        }

    }
}
