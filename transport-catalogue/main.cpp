// IO
#include <fstream>
#include <iostream>

// Local
#include "json_reader.h"

int main() {
    using namespace std;

    ifstream file("test.json");

    TransportCatalogue catalogue;
    JsonReader reader;

    reader.ReadData(file);
    reader.ProcessBaseRequests(catalogue);
    reader.ProcessStatRequests(catalogue, cout);
}