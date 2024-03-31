// IO
#include <fstream>
#include <iostream>

// Local
#include "json_reader.h"

int main() {
    using namespace std;

    // ifstream file("test.json"s);
    // ofstream svg_image("result.svg"s);

    TransportCatalogue catalogue;
    JsonReader reader;

    reader.ReadData(cin);
    renderer::MapRenderer renderer(
            reader.GetRenderSettings()
            );
    reader.ProcessBaseRequests(
            catalogue,
            renderer
            );
    RequestHandler handler(catalogue, renderer);
    reader.ProcessStatRequests(handler, cout);
}
