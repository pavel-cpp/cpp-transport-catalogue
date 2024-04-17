// IO
#include <fstream>
#include <iostream>

// Local
#include "json_reader.h"

int main() {
    using namespace std;

    ifstream ifile("test.json"s);
    ofstream ofile("out.json"s);

    TransportCatalogue catalogue;
    JsonReader reader;

    reader.ReadData(
#ifndef Debug
            cin
#else
            ifile
#endif
            );
    renderer::MapRenderer renderer(
            reader.GetRenderSettings()
            );
    reader.ProcessBaseRequests(
            catalogue,
            renderer
            );
    RequestHandler handler(catalogue, renderer);
    reader.ProcessStatRequests(
            handler,
#ifndef Debug
            cout
#else
            ofile
#endif
            );
}
