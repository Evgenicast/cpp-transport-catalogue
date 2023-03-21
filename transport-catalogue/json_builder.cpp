#include <stdexcept>
#include <utility>
#include <variant>

#include "json_builder.h"

namespace json
{
    using namespace std;
    using namespace std::literals;

    BuildConstructor::BuildConstructor(Builder & builder)
    : builder_(builder){}

    BuildDictArrayContext::BuildDictArrayContext(Builder & builder)
    : BuildConstructor(builder){}

    DictContext & BuildDictArrayContext::StartDict()
    {
        return builder_.StartDict();
    }

    ArrayContext & BuildDictArrayContext::StartArray()
    {
        return builder_.StartArray();
    }

    BuildKeyContext::BuildKeyContext(Builder & builder)
    : BuildConstructor(builder){}

    KeyContext & BuildKeyContext::Key(const string & Key)
    {
        return builder_.Key(Key);
    }

    Builder& BuildKeyContext::EndDict()
    {
        return builder_.EndDict();
    }

    KeyContext::KeyContext(Builder & builder)
    : BuildDictArrayContext(builder){}

    ValueKeyContext & KeyContext::Value(Node::Value value)
    {
        return builder_.Value(value);
    }

    ValueKeyContext::ValueKeyContext(Builder & builder)
    : BuildKeyContext(builder){}

    ValueArrayContext::ValueArrayContext(Builder & builder)
    : BuildDictArrayContext(builder){}

    ValueArrayContext & ValueArrayContext::Value(Node::Value Value)
    {
        return builder_.Value(Value);
    }

    Builder & ValueArrayContext::EndArray()
    {
        return builder_.EndArray();
    }

    DictContext::DictContext(Builder & builder)
    : BuildKeyContext(builder){}

    ArrayContext::ArrayContext(Builder & builder)
    : ValueArrayContext(builder){}

    Builder::Builder()
    : KeyContext(*this)
    , ValueKeyContext(*this)
    , DictContext(*this)
    , ArrayContext(*this){}

    bool Builder::ChooseException(const ObjectState & State)
    {
        switch (State)
        {
        case ObjectState::Root:
        {
            return !Root.IsNull();
            break;
        }
        case ObjectState::Value:
        {
            return !Root.IsNull() || !(NodesStack.empty()
                    || NodesStack.top()->IsArray()
                    || NodesStack.top()->IsString());
            break;
        }
        case ObjectState::Key:
        {
            return !Root.IsNull() || NodesStack.empty()
                    || !NodesStack.top()->IsMap();
            break;
        }
        case ObjectState::Map: // can be used Value to avoid copypaste but this way is more informative
        {
            return !Root.IsNull() || !(NodesStack.empty()
                   || NodesStack.top()->IsArray()
                   || NodesStack.top()->IsString());
            break;
        }
        case ObjectState::Array:
        {
            return !Root.IsNull() || !(NodesStack.empty()
                   || NodesStack.top()->IsArray()
                   || NodesStack.top()->IsString());
            break;
        }

        case ObjectState::EndMap:
        {
            return  !Root.IsNull() || NodesStack.empty()
                    || !NodesStack.top()->IsMap();
            break;
        }

        case ObjectState::EndArray:
        {
            return !Root.IsNull() || NodesStack.empty()
                    || !NodesStack.top()->IsArray();
            break;
        }

        default:
            return false;
            break;
        }
    }

    KeyContext & Builder::Key(const string & Key)
    {
        if (ChooseException(ObjectState::Key))
        {
            throw logic_error("Key cant' be placed outside Dictionary or after another Key"s);
        }
        NodesStack.push(new Node(Key));
        return *this;
    }

    Builder & Builder::Value(Node::Value Value)
    {
        if (ChooseException(ObjectState::Value))
        {
            throw std::logic_error("Value сan be only called after the constructor, Key or Array item "s);
        }
        PushNode(Value);
        return AddNode(*NodesStack.top());
    }

    DictContext & Builder::StartDict()
    {
        if (ChooseException(ObjectState::Map))
        {
            throw logic_error("StartDict сan be only called after the constructor, Key or Array item "s);
        }
        NodesStack.push(new Node(Dict()));
        return *this;
    }

    Builder & Builder::EndDict()
    {
        if (ChooseException(ObjectState::EndMap))
        {
            throw logic_error("EndDict сan't be called anywhere except Map"s);
        }
        return AddNode(*NodesStack.top());
    }

    ArrayContext & Builder::StartArray()
    {
        if (ChooseException(ObjectState::Array))
        {
            throw logic_error("StartArray сan be only called after the constructor, Key or Array item"s);
        }
        NodesStack.push(new Node(Array()));
        return *this;
    }

    Builder & Builder::EndArray()
    {
        if (ChooseException(ObjectState::EndArray))
        {
            throw logic_error("EndArray сan't be called anywhere except Array"s);
        }
        return AddNode(*NodesStack.top());
    }

    Node Builder::Build() const
    {
        if (Root.IsNull())
        {
            throw logic_error("Cant apply Build. The Object is not ready"s);
        }
        return Root;
    }

    Builder & Builder::AddNode(const Node & Object)
    {
        NodesStack.pop();
        if (NodesStack.empty())
        {
            Root = Object;
        }
        else if (NodesStack.top()->IsArray())
        {
            NodesStack.top()->AsArray().push_back(Object);
        }
        else
        {
            const Node & Key = *NodesStack.top();
            NodesStack.pop();
            NodesStack.top()->AsDict().emplace(Key.AsString(), Object);
        }
        return *this;
    }

    void Builder::PushNode(Node::Value Value)
    {
        visit([this](auto && Val)
        {
            NodesStack.push(new Node(Val));
        }, Value);
    }
}//namespace json

