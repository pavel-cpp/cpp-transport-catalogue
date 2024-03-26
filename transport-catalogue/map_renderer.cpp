#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
renderer::MapRenderer::MapRenderer(const Settings &settings) : settings_(settings) {}

void renderer::MapRenderer::AddBus(const Bus &bus) {
    buses_.insert(bus);
}

std::string renderer::MapRenderer::Render(std::ostream &out) {

}
