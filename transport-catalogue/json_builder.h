#pragma once

#include <string>
#include <vector>
#include "json.h"

namespace json {

    class Builder {
    private:
        class BaseContext;
        class DictValueContext;
        class DictItemContext;
        class ArrayItemContext;

    public:
        Builder();
        Node Build();
        DictValueContext Key(std::string key);
        BaseContext Value(Node::Value value);
        DictItemContext StartDict();
        ArrayItemContext StartArray();
        BaseContext EndDict();
        BaseContext EndArray();

    private:
        Node root_;
        std::vector<Node*> nodes_stack_;

        Node::Value& GetCurrentValue();
        const Node::Value& GetCurrentValue() const;

        void AssertNewObjectContext() const;
        void AddObject(Node::Value value, bool one_shot);

        // Key() → Value(), StartDict(), StartArray()
        // StartDict() → Key(), EndDict()
        // Key() → Value() → Key(), EndDict()
        // StartArray() → Value(), StartDict(), StartArray(), EndArray() 
        // StartArray() → Value() → Value(), StartDict(), StartArray(), EndArray() 

        class BaseContext {
        public:
            BaseContext(Builder& builder) : builder_(builder) {}
            Node Build() {
                return builder_.Build();
            }
            DictValueContext Key(std::string key) {
                return builder_.Key(std::move(key));
            }
            BaseContext Value(Node::Value value) {
                return builder_.Value(std::move(value));
            }
            DictItemContext StartDict() {
                return builder_.StartDict();
            }
            ArrayItemContext StartArray() {
                return builder_.StartArray();
            }
            BaseContext EndDict() {
                return builder_.EndDict();
            }
            BaseContext EndArray() {
                return builder_.EndArray();
            }
        private:
            Builder& builder_;
        };

        class DictValueContext : public BaseContext {
        public:
            DictValueContext(BaseContext base) : BaseContext(base) {}
            DictItemContext Value(Node::Value value) { return BaseContext::Value(std::move(value)); }
            Node Build() = delete;
            DictValueContext Key(std::string key) = delete;
            BaseContext EndDict() = delete;
            BaseContext EndArray() = delete;
        };

        class DictItemContext : public BaseContext {
        public:
            DictItemContext(BaseContext base) : BaseContext(base) {}
            Node Build() = delete;
            BaseContext Value(Node::Value value) = delete;
            BaseContext EndArray() = delete;
            DictItemContext StartDict() = delete;
            ArrayItemContext StartArray() = delete;
        };

        class ArrayItemContext : public BaseContext {
        public:
            ArrayItemContext(BaseContext base) : BaseContext(base) {}
            ArrayItemContext Value(Node::Value value) { return BaseContext::Value(std::move(value)); }
            Node Build() = delete;
            DictValueContext Key(std::string key) = delete;
            BaseContext EndDict() = delete;
        };
    };

}  // namespace json