// IO
#include <fstream>
#include <iostream>

// Local
#include "json_reader.h"

int main() {
    using namespace std;

    ifstream file("test.json"s);
    ofstream svg_image("result.svg"s);

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
    // reader.ProcessStatRequests(handler, cout);
    // cout << endl;
    handler.RenderMap().Render(svg_image);
}