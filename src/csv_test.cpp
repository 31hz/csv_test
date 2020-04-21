#include <iostream>
#include <string>
#include <sstream>
#include <getopt.h>
#include "workDispatcher.h"

// Validate that the supplied parameters are all within range
// Returns true or false
bool validate_options(const int concurrency, const int column, const std::string agg, const std::string csvglob) {
    bool result = false;

    if (
        (concurrency > 0) &&
        (column >= 0) &&
        (csvglob.length() > 0) &&
        ((agg == std::string("SUM")) || (agg == std::string("AVERAGE")))
    ) {
        result = true;
    }


    return result;
}

// Main entry
int main(int argc, char **argv) {

    // Parameter values
    int concurrency = -1;
    int column = -1;
    std::string csvglob;
    std::string agg;

    // Parse options
    struct option opts[] = {
        {"concurrency", required_argument, 0, 0},
        {"csvglob", required_argument, 0, 0},
        {"aggregation_function", required_argument, 0, 0},
        {"column", required_argument, 0, 0},
        { 0,0,0,0 }
    };

    int option_index = 0;
    int c;

    while ((c = getopt_long_only(argc, argv, "", opts, &option_index)) != -1) {
        // Note c is always zero since we only use long options

        if (optarg != NULL) {
            std::stringstream convertVal;

            convertVal << optarg;

	    switch(option_index) {
		case 0:
		convertVal >> concurrency; 
		break;

		case 1:
		convertVal >> csvglob;
		break;

		case 2:
		convertVal >> agg;
		break;

		case 3:
		convertVal >> column;
		break;

		default:
		break;
	    }
        }

    }

   // Now validate the options
   if (validate_options(concurrency, column, agg, csvglob)) {

       // The workDispatcher will manage the workload
       workDispatcher disp(concurrency, column, agg, csvglob);

       // Begin serving out the queue of jobs
       disp.run();

   } else {
       std::cerr << "Illegal or missing option values" << std::endl;
   }
}

