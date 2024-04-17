#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <list>
#include <optional>
#include <variant>

namespace svg {
    struct Rgb {
        uint8_t red{};
        uint8_t green{};
        uint8_t blue{};

        Rgb() = default;

        Rgb(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) {
        }
    };

    struct Rgba : Rgb {
        double opacity = 1.f;

        Rgba() = default;

        Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity) : Rgb(red, green, blue),
                                                                         opacity(opacity) {
        }
    };

    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

    const Color NoneColor{};

    inline std::ostream &operator<<(std::ostream &out, Rgb color) {
        using namespace std::literals;
        return out << "rgb("s
               << static_cast<int>(color.red) << ','
               << static_cast<int>(color.green) << ','
               << static_cast<int>(color.blue)
               << ')';
    }

    inline std::ostream &operator<<(std::ostream &out, Rgba color) {
        using namespace std::literals;
        return out << "rgba("s
               << static_cast<int>(color.red) << ','
               << static_cast<int>(color.green) << ','
               << static_cast<int>(color.blue) << ','
               << color.opacity
               << ')';
    }

    class ColorVisitor {
    public:
        explicit ColorVisitor(std::ostream &out) : out_(out) {
        }

        void operator()(std::monostate) const {
            using namespace std::literals;
            out_ << "none"sv;
        }

        void operator()(const std::string& color) const {
            out_ << color;
        }

        void operator()(Rgb rgb) const {
            out_ << rgb;
        }

        void operator()(Rgba rgba) const {
            out_ << rgba;
        }

    private:
        std::ostream &out_;
    };

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    inline std::ostream &operator<<(std::ostream &out, StrokeLineCap line_cap) {
        using namespace std::literals;
        switch (line_cap) {
            case StrokeLineCap::BUTT:
                return out << "butt"s;
            case StrokeLineCap::ROUND:
                return out << "round"s;
            case StrokeLineCap::SQUARE:
                return out << "square"s;
        }
        return out;
    }

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    inline std::ostream &operator<<(std::ostream &out, StrokeLineJoin line_join) {
        using namespace std::literals;
        switch (line_join) {
            case StrokeLineJoin::ARCS:
                return out << "arcs"s;
            case StrokeLineJoin::BEVEL:
                return out << "bevel"s;
            case StrokeLineJoin::MITER:
                return out << "miter"s;
            case StrokeLineJoin::MITER_CLIP:
                return out << "miter-clip"s;
            case StrokeLineJoin::ROUND:
                return out << "round"s;
        }
        return out;
    }

    struct Point {
        Point() = default;

        Point(double x, double y)
            : x(x), y(y) {
        }

        double x = 0;
        double y = 0;
    };

    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext {
        explicit RenderContext(std::ostream &out)
            : out(out) {
        }

        RenderContext(std::ostream &out, int indent_step, int indent = 0)
            : out(out), indent_step(indent_step), indent(indent) {
        }

        [[nodiscard]] RenderContext Indented() const {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream &out;
        int indent_step = 0;
        int indent = 0;
    };

    /*
     * Абстрактный базовый класс Object служит для унифицированного хранения
     * конкретных тегов SVG-документа
     * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
     */
    class Object {
    public:
        void Render(const RenderContext &context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext &context) const = 0;
    };

    template<typename Owner>
    class PathProps {
    public:
        Owner &SetFillColor(Color color) {
            fill_color_ = color;
            return AsOwner();
        }

        Owner &SetStrokeColor(Color color) {
            stroke_color_ = color;
            return AsOwner();
        }

        Owner &SetStrokeLineCap(StrokeLineCap line_cap) {
            stroke_line_cap_ = line_cap;
            return AsOwner();
        }

        Owner &SetStrokeLineJoin(StrokeLineJoin line_join) {
            stroke_line_join_ = line_join;
            return AsOwner();
        }

        Owner &SetStrokeWidth(double width) {
            stroke_width_ = width;
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        // Метод RenderAttrs выводит в поток общие для всех путей атрибуты fill и stroke
        void RenderAttrs(std::ostream &out) const {
            using namespace std::literals;

            if (fill_color_) {
                out << R"( fill=")";
                std::visit(ColorVisitor(out), *fill_color_);
                out << R"(")";
            }
            if (stroke_color_) {
                out << R"( stroke=")";
                std::visit(ColorVisitor(out), *stroke_color_);
                out << R"(")";
            }

            if (stroke_width_) {
                out << " stroke-width=\"" << *stroke_width_ << "\"";
            }

            if (stroke_line_cap_) {
                out << " stroke-linecap=\"" << *stroke_line_cap_ << R"(")";
            }

            if (stroke_line_join_) {
                out << " stroke-linejoin=\"" << *stroke_line_join_ << R"(")";
            }
        }

    private:
        Owner &AsOwner() {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner &>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<StrokeLineCap> stroke_line_cap_;
        std::optional<StrokeLineJoin> stroke_line_join_;
        std::optional<double> stroke_width_;
    };

    /*
     * Класс Circle моделирует элемент <circle> для отображения круга
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle &SetCenter(Point center);

        Circle &SetRadius(double radius);

    private:
        void RenderObject(const RenderContext &context) const override;

        Point center_;
        double radius_ = 1.0;
    };

    /*
     * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline &AddPoint(Point point);

    private:
        void RenderObject(const RenderContext &context) const override;

        std::list<Point> points_;
    };

    /*
     * Класс Text моделирует элемент <text> для отображения текста
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text final : public Object, public PathProps<Text> {
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text &SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text &SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text &SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text &SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text &SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text &SetData(std::string data);

    private:
        void RenderObject(const RenderContext &context) const override;

        Point position_{0.0, 0.0};
        Point offset_{0.0, 0.0};
        uint32_t size_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string data_;
    };

    class ObjectContainer {
    public:
        template<typename Obj>
        void Add(Obj obj) {
            AddPtr(std::make_unique<Obj>(std::move(obj)));
        }

        virtual void AddPtr(std::unique_ptr<Object> && /*obj*/) = 0;

    protected:
        ~ObjectContainer() = default;
    };

    class Drawable {
    public:
        virtual void Draw(ObjectContainer &container) const = 0;

        virtual ~Drawable() = default;
    };

    class Document : public ObjectContainer {
    public:
        /*
         Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
         Пример использования:
         Document doc;
         doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
        */

        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object> &&obj) override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream &out) const;

        // Прочие методы и данные, необходимые для реализации клaасса Document
    private:
        std::list<std::unique_ptr<Object> > objects_;
    };
} // namespace svg
