#ifndef WORKDISPATCHER_H
#define WORKDISPATCHER_H

#include <string>
#include <mutex>
#include <queue>
#include "worker.h"


class workDispatcher {
    private:

    int _concurrency;
    int _column;
    std::string _agg;
    std::string _csvglob;

    const int _WAIT_DELAY_MS = 100;

    std::queue<std::string> _fileQueue;
    std::mutex _fileMutex;

    std::mutex _threads_finished_mutex;
    int _threads_finished_count;

    // Do the file glob comparison and return count of found files
    int find_glob_files();

    public:

    std::string popOneFile();

    int get_finished_count();

    int inc_finished_count();

    workDispatcher(const int concurrency, const int column, const std::string agg, const std::string csvglob);

    ~workDispatcher();

    void run();
};
#endif
