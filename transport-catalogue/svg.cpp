#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext &context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }


    // ---------- Circle ------------------

    Circle &Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle &Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << "<circle";
        out << R"( cx=")" << center_.x << R"(" cy=")" << center_.y << R"(" )";
        out << R"(r=")" << radius_ << '"';
        RenderAttrs(out);
        out << "/>"sv;
    }

    Polyline &Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << "<polyline ";
        out << R"(points=")";
        bool is_first = true;
        for (const Point &point: points_) {
            if (is_first) {
                out << point.x << ","sv << point.y;
                is_first = false;
                continue;
            }
            out << ' ' << point.x << ","sv << point.y;
        }
        out << '"';
        RenderAttrs(out);
        out << "/>"sv;
    }


    Text &Text::SetPosition(Point pos) {
        position_ = pos;
        return *this;
    }

    Text &Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text &Text::SetFontSize(uint32_t size) {
        size_ = size;
        return *this;
    }

    Text &Text::SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    Text &Text::SetFontWeight(std::string font_weight) {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    Text &Text::SetData(std::string data) {
        data_ = std::move(data);
        return *this;
    }

    std::string EncodeHTMLString(std::string_view sv) {
        std::string result;
        for (char c: sv) {
            switch (c) {
                case '"':
                    result += "&quot;"sv;
                    break;
                case '<':
                    result += "&lt;"sv;
                    break;
                case '>':
                    result += "&gt;"sv;
                    break;
                case '&':
                    result += "&amp;"sv;
                    break;
                case '\'':
                    result += "&apos;"sv;
                    break;
                default:
                    result += c;
            }
        }
        return result;
    }

    void Text::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << "<text"sv;
        RenderAttrs(out);
        out << R"( x=")" << position_.x << "\" " << R"(y=")" << position_.y << "\" ";
        out << R"(dx=")" << offset_.x << "\" " << R"(dy=")" << offset_.y << "\" ";
        out << "font-size=\"" << size_ << "\"";
        if (!font_family_.empty()) {
            out << " font-family=\"" << font_family_ << "\"";
        }
        if (!font_weight_.empty()) {
            out << " font-weight=\"" << font_weight_ << "\"";
        }
        out << ">" << EncodeHTMLString(data_) << "</text>";
    }

    void Document::AddPtr(std::unique_ptr<Object> &&obj) {
        objects_.push_back(std::move(obj));
    }

    void Document::Render(std::ostream &out) const {
        out << R"(<?xml version="1.0" encoding="UTF-8" ?>)" << std::endl;
        out << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)" << std::endl;
        RenderContext context(out, 2, 2);
        for (const auto &object: objects_) {
            object->Render(context);
        }
        out << "</svg> "sv;
    }
} // namespace svg
