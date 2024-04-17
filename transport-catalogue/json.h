#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {
    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node final
        : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
    public:
        using variant::variant;
        using Value = variant;

        explicit Node(Value value) : variant(std::move(value)) {
        }

        [[nodiscard]] bool IsInt() const {
            return std::holds_alternative<int>(*this);
        }

        [[nodiscard]] int AsInt() const {
            using namespace std::literals;
            if (!IsInt()) {
                throw std::logic_error("Not an int"s);
            }
            return std::get<int>(*this);
        }

        [[nodiscard]] bool IsPureDouble() const {
            return std::holds_alternative<double>(*this);
        }

        [[nodiscard]] bool IsDouble() const {
            return IsInt() || IsPureDouble();
        }

        [[nodiscard]] double AsDouble() const {
            using namespace std::literals;
            if (!IsDouble()) {
                throw std::logic_error("Not a double"s);
            }
            return IsPureDouble() ? std::get<double>(*this) : AsInt();
        }

        [[nodiscard]] bool IsBool() const {
            return std::holds_alternative<bool>(*this);
        }

        [[nodiscard]] bool AsBool() const {
            using namespace std::literals;
            if (!IsBool()) {
                throw std::logic_error("Not a bool"s);
            }

            return std::get<bool>(*this);
        }

        [[nodiscard]] bool IsNull() const {
            return std::holds_alternative<std::nullptr_t>(*this);
        }

        [[nodiscard]] bool IsArray() const {
            return std::holds_alternative<Array>(*this);
        }

        [[nodiscard]] const Array& AsArray() const {
            using namespace std::literals;
            if (!IsArray()) {
                throw std::logic_error("Not an array"s);
            }

            return std::get<Array>(*this);
        }

        [[nodiscard]] bool IsString() const {
            return std::holds_alternative<std::string>(*this);
        }

        [[nodiscard]] const std::string& AsString() const {
            using namespace std::literals;
            if (!IsString()) {
                throw std::logic_error("Not a string"s);
            }

            return std::get<std::string>(*this);
        }

        [[nodiscard]] bool IsDict() const {
            return std::holds_alternative<Dict>(*this);
        }

        [[nodiscard]] const Dict& AsDict() const {
            using namespace std::literals;
            if (!IsDict()) {
                throw std::logic_error("Not a dict"s);
            }

            return std::get<Dict>(*this);
        }

        bool operator==(const Node& rhs) const {
            return GetValue() == rhs.GetValue();
        }

        [[nodiscard]] const Value& GetValue() const {
            return *this;
        }

        Value& GetValue() {
            return *this;
        }
    };

    inline bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    class Document {
    public:
        explicit Document(Node root)
            : root_(std::move(root)) {
        }

        [[nodiscard]] const Node& GetRoot() const {
            return root_;
        }

    private:
        Node root_;
    };

    inline bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    inline bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);
}
