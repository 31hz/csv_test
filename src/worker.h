#ifndef WORKER_H
#define WORKER_H

#include <string>
#include <vector>
#include "workDispatcher.h"

class worker {
    private:

    void process_one_line(std::vector<std::string> &fields, const std::string one_line);

    void process_one_file(const std::string nextFile, const int column, const std::string agg);


    public:

    void operator() (class workDispatcher *disp, const int column, const std::string agg);

    worker();

    ~worker();
};

#endif
