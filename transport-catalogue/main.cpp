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
    renderer::Settings settings;
    reader.ProcessRenderSettings(settings);
    renderer::MapRenderer renderer(settings);
    reader.ProcessBaseRequests(
        catalogue,
        renderer
        );
    RequestHandler handler(catalogue, renderer);
    reader.ProcessStatRequests(handler, cout);
    handler.RenderMap().Render(cout);
}