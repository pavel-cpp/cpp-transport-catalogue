// IO
#include <fstream>
#include <iostream>

// Other
#include <string>

// Local
#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    ifstream file("test.txt");
    TransportCatalogue catalogue;

    int base_request_count;
    file >> base_request_count >> ws;

    {
        InputReader reader;
        for (int i = 0; i < base_request_count; ++i) {
            string line;
            getline(file, line);
            reader.ParseLine(line);
        }
        reader.ApplyCommands(catalogue);
    }

    int stat_request_count;
    file >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(file, line);
        ParseAndPrintStat(catalogue, line, cout);
    }
}