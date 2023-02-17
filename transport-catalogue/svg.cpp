#include "svg.h"

namespace svg
{
	using namespace std::literals;

	Point::Point(const double& x, const double& y)
    : x(x), y(y){}

	RenderContext::RenderContext(std::ostream& out)
    : out(out){}

	RenderContext::RenderContext(std::ostream& out, const int& indent_step, const int& indent)
    : out(out), indent_step(indent_step), indent(indent) {}

	RenderContext RenderContext::Indented() const
	{
		return { out, indent_step, indent + indent_step };
	}

	void RenderContext::RenderIndent() const
	{
		for (int i = 0; i < indent; ++i)
		{
			out.put(' ');
		}
	}

	void Object::Render(const RenderContext& context) const
	{
		context.RenderIndent();
		RenderObject(context);
		context.out << "\n"sv;
	}

	Rgb::Rgb(const uint8_t& r, const uint8_t& g, const uint8_t& b)
    : red(r), green(g), blue(b) {}

	Rgba::Rgba(const uint8_t& r, const uint8_t& g, const uint8_t& b, const double& o)
    : Rgb(r, g, b), opacity(o) {}

    std::ostream & operator<<(std::ostream & out, const StrokeLineCap & line_cap)
	{
		switch (line_cap)
		{
		case StrokeLineCap::BUTT:
			out << "butt"sv;
			break;
		case StrokeLineCap::ROUND:
			out << "round"sv;
			break;
		case StrokeLineCap::SQUARE:
			out << "square"sv;
			break;
		default:
			break;
		}
		return out;
	}

    std::ostream & operator<<(std::ostream & out, const StrokeLineJoin & line_join)
	{
		switch (line_join)
		{
		case StrokeLineJoin::ARCS:
			out << "arcs"sv;
			break;
		case StrokeLineJoin::BEVEL:
			out << "bevel"sv;
			break;
		case StrokeLineJoin::MITER:
			out << "miter"sv;
			break;
		case StrokeLineJoin::MITER_CLIP:
			out << "miter-clip"sv;
			break;
		case StrokeLineJoin::ROUND:
			out << "round"sv;
			break;
		default:
			break;
		}
		return out;
	}

    Circle & Circle::SetCenter(const Point & Center_)
	{
        Center = Center_;
		return *this;
	}

    Circle & Circle::SetRadius(const double & Radius_)
	{
        Radius = Radius_;
		return *this;
	}

    void Circle::RenderObject(const RenderContext & context) const
	{
        auto & out = context.out;
        out << "<circle cx=\""sv << Center.x << "\" cy=\""sv << Center.y << "\" "sv;
        out << "r=\""sv << Radius << "\""sv;
		RenderAttrs(out);
		out << "/>"sv;
	}

    Polyline & Polyline::AddPoint(const Point & Point)
	{
        Points.emplace_back(Point);
		return *this;
	}

    void Polyline::RenderObject(const RenderContext & context) const
	{
		auto& out = context.out;
		out << "<polyline points=\""sv;
        const size_t PointsSize = Points.size();
        for (size_t i = 0; i < PointsSize; ++i)
		{
            const auto& point = Points[i];
			if (i)
			{
				out << " "sv;
			}
			out << point.x << ","sv << point.y;
		}
		out << "\""sv;
		RenderAttrs(out);
		out << "/>"sv;
	}

    Text & Text::SetPosition(const Point & Pos_)
    {
        Position = Pos_;
        return *this;
    }

    Text & Text::SetOffset(const Point & Offset_)
    {
        Offset = Offset_;
        return *this;
    }

    Text & Text::SetFontSize(const uint32_t & FontSize_)
    {
        FontSize = FontSize_;
        return *this;
    }

    Text & Text::SetFontFamily(const std::string & FontFamily_)
    {
        FontFamily = FontFamily_;
        return *this;
    }

    Text & Text::SetFontWeight(const std::string & FontWeight_)
    {
        FontWeight = FontWeight_;
        return *this;
    }

    Text & Text::SetData(const std::string & Data_)
    {
        Data = Data_;
        return *this;
    }

    void Text::RenderObject(const RenderContext & context) const
    {
        auto& out = context.out;
        out << "<text"sv;
        RenderAttrs(out);
        out << " x=\""sv << Position.x << "\" y=\""sv << Position.y << "\""sv;
        out << " dx=\""sv << Offset.x << "\" dy=\""sv << Offset.y << "\""sv;
        out << " font-size=\""sv << FontSize << "\""sv;
        if (FontFamily)
        {
            out << " font-family=\""sv << *FontFamily << "\""sv;
        }
        if (FontWeight)
        {
            out << " font-weight=\""sv << *FontWeight << "\""sv;
        }
        out << ">"sv;
        std::string Text;
        for (const char Letter : Data)
        {
            switch (Letter)
            {
            case '\"':
                Text += "&quot;"s;
                break;
            case '\'':
                Text += "&apos;"s;
                break;
            case '<':
                Text += "&lt;"s;
                break;
            case '>':
                Text += "&gt;"s;
                break;
            case '&':
                Text += "&amp"s;
                break;
            default:
                Text += Letter;
            }
        }
        out << std::move(Text) << "</text>"sv;
    }

    void Document::AddPtr(std::shared_ptr<Object> && obj)
    {
        Objects.emplace_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;
        for (const auto & Item : Objects)
        {
            Item.get()->Render(RenderContext(out, 2, 2));
        }
        out << "</svg>"sv;
    }
} // namespace svg
