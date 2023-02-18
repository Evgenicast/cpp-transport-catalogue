#ifndef JSON_H
#define JSON_H
#pragma once
#include <iostream>
#include <map>
#include <variant>
#include <vector>

namespace json
{
    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError
        : public std::runtime_error
    {
    public:

        using runtime_error::runtime_error;
    };

    class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>
    {
    public:

        using variant::variant;
        using Value = variant;

        bool IsInt() const;
        int AsInt()  const;
        bool IsPureDouble() const;
        bool IsDouble() const;
        double AsDouble() const;
        bool IsBool() const;
        bool AsBool() const;
        bool IsNull() const;
        bool IsArray() const;
        const Array & AsArray() const;
        bool IsString() const;
        const std::string & AsString() const;
        bool IsMap() const;
        const Dict & AsDict() const;
        bool operator==(const Node& rhs) const;
        const Value & GetValue() const;
    };

    inline bool operator!=(const Node& lhs, const Node& rhs)
    {
        return !(lhs == rhs);
    }

    class Document
    {
    private:

        Node root_;

    public:

        explicit Document(Node root)
            : root_(std::move(root)) {}

        const Node& GetRoot() const
        {
            return root_;
        }
    };

    inline bool operator==(const Document & lhs, const Document & rhs)
    {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    inline bool operator!=(const Document & lhs, const Document & rhs)
    {
        return !(lhs == rhs);
    }

    Document Load(std::istream & Input);

    void Print(const Document & Doc, std::ostream & Output);

} // namespace json

#endif // JSON_H
