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

    class Node final {
    public:
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node() = default;

        Node(std::nullptr_t)
            : value_(nullptr) {
        }

        Node(int val)
            : value_(val) {
        }

        Node(double val)
            : value_(val) {
        }

        Node(std::string val)
            : value_(std::move(val)) {
        }

        Node(Array val)
            : value_(std::move(val)) {
        }

        Node(Dict val)
            : value_(std::move(val)) {
        }

        Node(bool val)
            : value_(val) {
        }

        bool IsInt() const {
            return std::holds_alternative<int>(value_);
        }

        int AsInt() const {
            using namespace std::literals;
            if (!IsInt()) {
                throw std::logic_error("Not an int"s);
            }
            return std::get<int>(value_);
        }

        bool IsPureDouble() const {
            return std::holds_alternative<double>(value_);
        }

        bool IsDouble() const {
            return IsInt() || IsPureDouble();
        }

        double AsDouble() const {
            using namespace std::literals;
            if (!IsDouble()) {
                throw std::logic_error("Not a double"s);
            }
            return IsPureDouble() ? std::get<double>(value_) : AsInt();
        }

        bool IsBool() const {
            return std::holds_alternative<bool>(value_);
        }

        bool AsBool() const {
            using namespace std::literals;
            if (!IsBool()) {
                throw std::logic_error("Not a bool"s);
            }

            return std::get<bool>(value_);
        }

        bool IsNull() const {
            return std::holds_alternative<std::nullptr_t>(value_);
        }

        bool IsArray() const {
            return std::holds_alternative<Array>(value_);
        }

        const Array &AsArray() const {
            using namespace std::literals;
            if (!IsArray()) {
                throw std::logic_error("Not an array"s);
            }

            return std::get<Array>(value_);
        }

        bool IsString() const {
            return std::holds_alternative<std::string>(value_);
        }

        const std::string &AsString() const {
            using namespace std::literals;
            if (!IsString()) {
                throw std::logic_error("Not a string"s);
            }

            return std::get<std::string>(value_);
        }

        bool IsMap() const {
            return std::holds_alternative<Dict>(value_);
        }

        const Dict &AsMap() const {
            using namespace std::literals;
            if (!IsMap()) {
                throw std::logic_error("Not a map"s);
            }

            return std::get<Dict>(value_);
        }

        bool operator==(const Node &rhs) const {
            return value_ == rhs.value_;
        }

        const Value &GetValue() const {
            return value_;
        }

    private:
        Value value_;
    };

    inline bool operator!=(const Node &lhs, const Node &rhs) {
        return !(lhs == rhs);
    }

    class Document {
    public:
        explicit Document(Node root)
            : root_(std::move(root)) {
        }

        const Node &GetRoot() const {
            return root_;
        }

    private:
        Node root_;
    };

    inline bool operator==(const Document &lhs, const Document &rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    inline bool operator!=(const Document &lhs, const Document &rhs) {
        return !(lhs == rhs);
    }

    Document Load(std::istream &input);

    void Print(const Document &doc, std::ostream &output);
} // namespace json
