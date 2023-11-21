// IO
#include <fstream>
#include <iostream>

// Local
#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    ifstream file("test.txt");
    TransportCatalogue catalogue;

    {
        InputReader reader;
        reader.ReadCommands(file);
        reader.ApplyCommands(catalogue);
    }

    {
        StatReader reader;
        reader.ReadRequests(file);
        reader.ExecuteAndPrintStat(catalogue, cout);
    }
}