#pragma once
#include <memory>
#include <stack>
#include <string>
#include <algorithm>

#include "json.h"

namespace json
{
    class BuildConstructor;
    class Builder;

    struct BuildDictArrayContext;
    struct BuildKeyContext;
    struct KeyContext;
    struct ValueKeyContext;
    struct ValueArrayContext;
    struct DictContext;
    struct ArrayContext;

    enum class ObjectState
    {
        Root,
        Value,
        Key,
        Map,
        Array,
        EndMap,
        EndArray
    };

    class BuildConstructor
    {
    public:
        explicit BuildConstructor(Builder & builder);

    protected:
        Builder & builder_;
    };

    struct BuildDictArrayContext : BuildConstructor
    {
        explicit BuildDictArrayContext(Builder & builder);
        DictContext & StartDict();
        ArrayContext & StartArray();
    };

    struct BuildKeyContext : BuildConstructor
    {
        explicit BuildKeyContext(Builder & builder);
        KeyContext & Key(const std::string & Key);
        Builder & EndDict();
    };

    struct KeyContext : BuildDictArrayContext
    {
        explicit KeyContext(Builder & builder);
        ValueKeyContext & Value(Node::Value Value);
    };

    struct ValueKeyContext : BuildKeyContext
    {
        explicit ValueKeyContext(Builder & builder);
    };

    struct ValueArrayContext : BuildDictArrayContext
    {
        explicit ValueArrayContext(Builder & builder);
        ValueArrayContext & Value(Node::Value value);
        Builder & EndArray();
    };

    struct DictContext : BuildKeyContext
    {
        explicit DictContext(Builder & builder);
    };

    struct ArrayContext : ValueArrayContext
    {
        explicit ArrayContext(Builder & builder);
    };

    class Builder :
    public KeyContext,
    public ValueKeyContext,
    public DictContext,
    public ArrayContext
    {
    private:
        Builder & AddNode(const Node & Object);
        void PushNode(Node::Value Value);
        Node Root = nullptr;
        std::stack<Node*> NodesStack;
    public:
        Builder();
        ~Builder() = default;

        Builder(const Builder &) = delete;
        Builder & operator=(const Builder &) = delete;
        Builder(Builder &&) noexcept = delete;
        Builder & operator=(Builder &&) noexcept = delete;

        bool ChooseException(const ObjectState & State);

        KeyContext & Key(const std::string & Key);
        Builder & Value(Node::Value Value);
        DictContext & StartDict();
        Builder & EndDict();
        ArrayContext & StartArray();
        Builder & EndArray();
        Node Build() const;
    };
}
