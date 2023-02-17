#include "json.h"
#include <iterator>
#include <cctype>

namespace json
{
    using namespace std::literals;

    bool Node::IsInt() const
    {
        return std::holds_alternative<int>(*this);
    }

    int Node::AsInt() const
    {
        return (!IsInt()) ? throw std::logic_error("Error! An Integer type is expected") : std::get<int>(*this);
    }

    bool Node::IsPureDouble() const
    {
        return std::holds_alternative<double>(*this);
    }

    double Node::AsDouble() const
    {
        return (!IsDouble()) ? throw std::logic_error("Error! A Double type is expected") : IsPureDouble() ? std::get<double>(*this) : AsInt();
    }

    bool Node::IsBool() const
    {
        return std::holds_alternative<bool>(*this);
    }

    bool Node::AsBool() const
    {
        return (!IsBool()) ? throw std::logic_error("Error! A Boolean type is expected") : std::get<bool>(*this);
    }

    bool Node::IsNull() const
    {
        return std::holds_alternative<std::nullptr_t>(*this);
    }

    bool Node::IsArray() const
    {
        return std::holds_alternative<Array>(*this);
    }

    const Array & Node::AsArray() const
    {
        return (!IsArray()) ? throw std::logic_error("Error! An Array type is expected") : std::get<Array>(*this);
    }

    bool Node::IsString() const
    {
        return std::holds_alternative<std::string>(*this);
    }

    const std::string & Node::AsString() const
    {
        return (!IsString()) ? throw std::logic_error("Error! A String type is expected") : std::get<std::string>(*this);
    }

    bool Node::IsMap() const
    {
        return std::holds_alternative<Dict>(*this);
    }

    const Dict & Node::AsDict() const
    {
        return (!IsMap()) ? throw std::logic_error("Error! A Dict type is expected") : std::get<Dict>(*this);
    }

    bool Node::operator==(const Node & rhs) const
    {
        return GetValue() == rhs.GetValue();
    }

    const Node::Value & Node::GetValue() const
    {
        return *this;
    }

    Node LoadNode(std::istream & input);
    Node LoadString(std::istream & input);

    std::string LoadLiteral(std::istream & input)
    {
        std::string Dummy;
        while (std::isalpha(input.peek()))
        {
            Dummy.push_back(static_cast<char>(input.get()));
        }
        return Dummy;
    }

    Node LoadArray(std::istream & input)
    {
        std::vector<Node> Result;

        for (char c; input >> c && c != ']';)
        {
            if (c != ',')
            {
                input.putback(c);
            }
            Result.push_back(LoadNode(input));
        }
        if (!input)
        {
            throw ParsingError("Failed to load Array"s);
        }
        return Node(std::move(Result));
    }

    Node LoadDict(std::istream & input)
    {
        Dict Dict;
        for (char c; input >> c && c != '}';)
        {
            if (c == '"')
            {
                std::string Key = LoadString(input).AsString();
                if (input >> c && c == ':')
                {
                    if (Dict.find(Key) != Dict.end())
                    {
                        throw ParsingError("Duplicate key '"s + Key + "' has been passed");
                    }
                    Dict.emplace(std::move(Key), LoadNode(input));
                }
                else
                {
                    throw ParsingError(": is expected but '"s + c + "' has been passed"s);
                }
            }
            else if (c != ',')
            {
                throw ParsingError(R"(',' is expected but ')"s + c + "' has been passed"s);
            }
        }
        if (!input)
        {
            throw ParsingError("Failed to load Dictionary"s);
        }
        return Node(std::move(Dict));
    }

    Node LoadString(std::istream & input)
    {
        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        std::string s;
        while (true)
        {
            if (it == end)
            {
                throw ParsingError("Failed to load String"s);
            }
            const char ch = *it;
            if (ch == '"')
            {
                ++it;
                break;
            }
            else if (ch == '\\')
            {
                ++it;
                if (it == end)
                {
                    throw ParsingError("Failed to load String"s);
                }
                const char escaped_char = *(it);
                switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                }
            }
            else if (ch == '\n' || ch == '\r')
            {
                throw ParsingError("Unexpected end of line"s);
            }
            else
            {
                s.push_back(ch);
            }
            ++it;
        }
        return Node(std::move(s));
    }

    Node LoadBool(std::istream & input)
    {
        const auto s = LoadLiteral(input);
        if (s == "true"sv)
        {
            return Node{ true };
        }
        else if (s == "false"sv)
        {
            return Node{ false };
        }
        else
        {
            throw ParsingError("Failed to load '"s + s + "' as bool"s);
        }
    }

    Node LoadNull(std::istream& input)
    {
        if (auto literal = LoadLiteral(input); literal == "null"sv)
        {
            return Node{ nullptr };
        }
        else
        {
            throw ParsingError("Failed to load '"s + literal + "' as null"s);
        }
    }

    Node LoadNumber(std::istream& input)
    {
        std::string parsed_num;

        auto read_char = [&parsed_num, &input]
        {
            parsed_num += static_cast<char>(input.get());
            if (!input)
            {
                throw ParsingError("Failed to read number from stream"s);
            }
        };

        auto read_digits = [&input, read_char]
        {
            if (!std::isdigit(input.peek()))
            {
                throw ParsingError("A digit is expected"s);
            }
            while (std::isdigit(input.peek()))
            {
                read_char();
            }
        };

        if (input.peek() == '-')
        {
            read_char();
        }
        if (input.peek() == '0')
        {
            read_char();
        }
        else
        {
            read_digits();
        }

        bool is_int = true;
        if (input.peek() == '.')
        {
            read_char();
            read_digits();
            is_int = false;
        }

        if (int ch = input.peek(); ch == 'e' || ch == 'E')
        {
            read_char();
            if (ch = input.peek(); ch == '+' || ch == '-')
            {
                read_char();
            }
            read_digits();
            is_int = false;
        }

        try
        {
            if (is_int) {
                try {
                    return std::stoi(parsed_num);
                }
                catch (...)
                {

                }
            }
            return std::stod(parsed_num);
        }
        catch (...)
        {
            throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
        }
    }

    Node LoadNode(std::istream& input)
    {
        char c;
        if (!(input >> c))
        {
            throw ParsingError("Unexpected EOF"s);
        }
        switch (c)
        {
        case '[':
            return LoadArray(input);
        case '{':
            return LoadDict(input);
        case '"':
            return LoadString(input);
        case 't':
            [[fallthrough]];
        case 'f':
            input.putback(c);
            return LoadBool(input);
        case 'n':
            input.putback(c);
            return LoadNull(input);
        default:
            input.putback(c);
            return LoadNumber(input);
        }
    }

    struct PrintContext
    {
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const
        {
            for (int i = 0; i < indent; ++i)
            {
                out.put(' ');
            }
        }

        PrintContext Indented() const
        {
            return { out, indent_step, indent_step + indent };
        }
    };

    void PrintNode(const Node & Node_, const PrintContext & CTX);

    template <typename Value>
    void PrintValue(const Value & Value_, const PrintContext & CTX)
    {
        CTX.out << Value_;
    }

    void PrintString(const std::string & Value, std::ostream & out)
    {
        out.put('"');
        for (const char c : Value)
        {
            switch (c)
            {
            case '\r':
                out << "\\r"sv;
                break;
            case '\n':
                out << "\\n"sv;
                break;
            case '"':
                [[fallthrough]];
            case '\\':
                out.put('\\');
                [[fallthrough]];
            default:
                out.put(c);
                break;
            }
        }
        out.put('"');
    }

    template <>
    void PrintValue<std::string>(const std::string & Value, const PrintContext & CTX)
    {
        PrintString(Value, CTX.out);
    }

    template <>
    void PrintValue<std::nullptr_t>(const std::nullptr_t &, const PrintContext & CTX)
    {
        CTX.out << "null"sv;
    }

    template <>
    void PrintValue<bool>(const bool & Value, const PrintContext & CTX)
    {
        CTX.out << (Value ? "true"sv : "false"sv);
    }

    template <>
    void PrintValue<Array>(const Array & Nodes_, const PrintContext & CTX)
    {
        std::ostream& out = CTX.out;
        out << "[\n"sv;
        bool First = true;
        auto InnerCTX = CTX.Indented();
        for (const Node & Item : Nodes_)
        {
            if (First)
            {
                First = false;
            }
            else
            {
                out << ",\n"sv;
            }
            InnerCTX.PrintIndent();
            PrintNode(Item, InnerCTX);
        }
        out.put('\n');
        CTX.PrintIndent();
        out.put(']');
    }

    template <>
    void PrintValue<Dict>(const Dict & Nodes_, const PrintContext & CTX)
    {
        std::ostream & out = CTX.out;
        out << "{\n"sv;
        bool First = true;
        auto InnerCTX = CTX.Indented();
        for (const auto & [Key, Node] : Nodes_)
        {
            if (First)
            {
                First = false;
            }
            else
            {
                out << ",\n"sv;
            }
            InnerCTX.PrintIndent();
            PrintString(Key, CTX.out);
            out << ": "sv;
            PrintNode(Node, InnerCTX);
        }
        out.put('\n');
        CTX.PrintIndent();
        out.put('}');
    }

    void PrintNode(const Node & Node_, const PrintContext & CTX)
    {
        std::visit([&CTX](const auto & Value)
        {
            PrintValue(Value, CTX);
        },  Node_.GetValue());
    }

    bool Node::IsDouble() const
    {
        bool PureDouble = IsPureDouble();
        if (PureDouble == true)
        {
            return PureDouble;
        }
        PureDouble = IsInt();
        return PureDouble;
    }

    Document Load(std::istream & Input)
    {
        return Document{ LoadNode(Input) };
    }

    void Print(const Document & Doc, std::ostream & Output)
    {
        PrintNode(Doc.GetRoot(), PrintContext{ Output });
    }

} // namespace json
