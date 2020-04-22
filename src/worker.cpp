#include "worker.h"
#include "workDispatcher.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/tokenizer.hpp>

void worker::operator() (class workDispatcher *disp, const int column, const std::string agg) {
    std::string nextFile;
    bool done = false;


    while (!done) {
        nextFile = disp->popOneFile();

        if (nextFile.length() == 0) {
            done = true;
        } else {
            process_one_file(nextFile, column, agg);

        }
    }
    disp->inc_finished_count();
}

void worker::process_one_file(const std::string nextFile, const int column, const std::string agg) {

    std::fstream my_csv;
    std::string one_line;
    std::vector<std::string> fields;
    double sum_of_records = 0;
    int num_records = 0;
    std::string one_item;
    double one_float;

    my_csv.open(nextFile, std::ios::in);
   

   if (my_csv.is_open()) {
       while (getline(my_csv, one_line)) {
           process_one_line(fields, one_line);

           // Examine the data field
           if (fields.size() > column) {
           one_item = fields.at(column);

           if (one_item.length() > 0) { // Not counting blank lines/fields
               std::stringstream convertVal;

               num_records++;
               convertVal << one_item;
               convertVal >> one_float;
               sum_of_records += one_float;
           }
           }
       }
       my_csv.close();

       // Print the final results
       if (num_records > 0) { // Ignore degenerate case of empty file or missing column
       std::cout << nextFile << " ";

       if (agg == std::string("SUM")) {
           std::cout << sum_of_records << std::endl;
       } else {
           // We previously validated that AVERAGE is the only other possible value
           std::cout << (sum_of_records / num_records) << std::endl;
       }
       }
   }
}

// Parse one line
// fields is updated by reference
void worker::process_one_line(std::vector<std::string> &fields, const std::string one_line) {

    boost::tokenizer< boost::escaped_list_separator<char> > tok(one_line);
    fields.assign(tok.begin(),tok.end());
}

worker::worker() {
}

worker::~worker() {
}
