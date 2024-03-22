#include "svg.h"

#include <cmath>

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
        RenderAttrs(out);
        out << " cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        out << "/>"sv;
    }

    Polyline &Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << "<polyline ";
        RenderAttrs(out);
        out << " points=\""sv;
        bool is_first = true;
        for (const Point &point: points_) {
            if (is_first) {
                out << point.x << ","sv << point.y;
                is_first = false;
                continue;
            }
            out << ' ' << point.x << ","sv << point.y;
        }
        out << "\"/>"sv;
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
        out << "<text "sv;
        RenderAttrs(out);
        out << "x=\""sv << position_.x << "\" " << "y=\""sv << position_.y << "\" ";
        out << "dx=\""sv << offset_.x << "\" " << "dy=\""sv << offset_.y << "\" ";
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
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        RenderContext context(out, 2, 2);
        for (const auto &object: objects_) {
            object->Render(context);
        }
        out << "</svg> "sv;
    }
}  // namespace svg

namespace shapes {
    Star::Star(svg::Point center, double outer_radius, double inner_radius, int num_rays)
            : center_(center),
              outer_radius_(outer_radius),
              inner_radius_(inner_radius),
              num_rays_(num_rays) {}

    void Star::Draw(svg::ObjectContainer &container) const {
        svg::Polyline polyline;
        for (int i = 0; i <= num_rays_; ++i) {
            double angle = 2 * M_PI * (i % num_rays_) / num_rays_;
            polyline.AddPoint({center_.x + outer_radius_ * sin(angle), center_.y - outer_radius_ * cos(angle)});
            if (i == num_rays_) {
                break;
            }
            angle += M_PI / num_rays_;
            polyline.AddPoint({center_.x + inner_radius_ * sin(angle), center_.y - inner_radius_ * cos(angle)});
        }
        polyline.SetFillColor("red").SetStrokeColor("black");
        container.Add(polyline);
    }

    Triangle::Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
            : p1_(p1), p2_(p2), p3_(p3) {
    }

    void Triangle::Draw(svg::ObjectContainer &container) const {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }

    Snowman::Snowman(svg::Point head_center, double head_radius) : head_center_(head_center),
                                                                   head_radius_(head_radius) {}

    void Snowman::Draw(svg::ObjectContainer &container) const {
        using svg::Circle;
        container.Add(  //
                Circle()
                        .SetCenter({head_center_.x, head_center_.y + 5 * head_radius_})
                        .SetRadius(2 * head_radius_)
                        .SetFillColor(FILL_COLOR)
                        .SetStrokeColor(STROKE_COLOR)
        );

        container.Add(  //
                Circle()
                        .SetCenter({head_center_.x, head_center_.y + 2 * head_radius_})
                        .SetRadius(1.5 * head_radius_)
                        .SetFillColor(FILL_COLOR)
                        .SetStrokeColor(STROKE_COLOR)
        );
        container.Add(
                Circle()
                        .SetCenter(head_center_)
                        .SetRadius(head_radius_)
                        .SetFillColor(FILL_COLOR)
                        .SetStrokeColor(STROKE_COLOR)
        );
    }
} // namespace shape