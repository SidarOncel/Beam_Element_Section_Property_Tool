#include "SectionExport.hpp"

#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace
{
    struct NamedValue
    {
        std::string name;
        double value = 0.0;
        std::string unit;
    };

    std::string formatNumber(double value, int precision)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(precision) << value;
        return stream.str();
    }

    std::vector<NamedValue> inputFields(const SectionInput& input)
    {
        switch (input.type)
        {
        case SectionType::HSection:
            return {
                {"Width", input.width, "mm"},
                {"Height", input.height, "mm"},
                {"Flange Thickness", input.flangeThickness, "mm"},
                {"Web Thickness", input.webThickness, "mm"}
            };
        case SectionType::BoxSection:
            return {
                {"Width", input.width, "mm"},
                {"Height", input.height, "mm"},
                {"Top Flange Thickness", input.topFlangeThickness, "mm"},
                {"Bottom Flange Thickness", input.bottomFlangeThickness, "mm"},
                {"Left Web Thickness", input.leftWebThickness, "mm"},
                {"Right Web Thickness", input.rightWebThickness, "mm"}
            };
        case SectionType::PipeSection:
            return {
                {"Outer Diameter", input.diameter, "mm"},
                {"Wall Thickness", input.wallThickness, "mm"}
            };
        case SectionType::GirderSection:
            return {
                {"A", input.A, "mm"},
                {"B", input.B, "mm"},
                {"G", input.G, "mm"},
                {"D", input.D, "mm"},
                {"e", input.e, "mm"},
                {"f", input.f, "mm"},
                {"H", input.H, "mm"},
                {"W", input.W, "mm"},
                {"M", input.M, "mm"},
                {"N", input.N, "mm"},
                {"p", input.p, "mm"},
                {"s", input.s, "mm"},
                {"t", input.t, "mm"},
                {"u", input.u, "mm"},
                {"M1", input.M1, "mm"},
                {"k", input.k, "mm"},
                {"k1", input.k1, "mm"},
                {"h", input.h, "mm"},
                {"h1", input.h1, "mm"}
            };
        }

        return {};
    }

    std::vector<NamedValue> propertyFields(const SectionProperties& properties)
    {
        return {
            {"Area", properties.Area, "mm^2"},
            {"cy", properties.cy, "mm"},
            {"cz", properties.cz, "mm"},
            {"Jz", properties.Jz, "mm^4"},
            {"Jy", properties.Jy, "mm^4"},
            {"Jyz", properties.Jyz, "mm^4"},
            {"Jzo", properties.Jzo, "mm^4"},
            {"Jyo", properties.Jyo, "mm^4"},
            {"Jx", properties.Jx, "mm^4"},
            {"Az", properties.Az, "mm^2"},
            {"Ay", properties.Ay, "mm^2"}
        };
    }

    std::string femKey(const std::string& label)
    {
        std::string key;
        key.reserve(label.size());

        for (char ch : label)
        {
            if (ch >= 'a' && ch <= 'z')
            {
                key.push_back(static_cast<char>(ch - ('a' - 'A')));
            }
            else if (ch >= 'A' && ch <= 'Z')
            {
                key.push_back(ch);
            }
            else if (ch >= '0' && ch <= '9')
            {
                key.push_back(ch);
            }
            else
            {
                key.push_back('_');
            }
        }

        return key;
    }

    void appendNamedValueBlock(std::ostringstream& stream,
                               const std::vector<NamedValue>& fields,
                               int precision,
                               const std::string& prefix)
    {
        for (const NamedValue& field : fields)
        {
            stream << prefix << field.name << ": "
                   << formatNumber(field.value, precision) << ' ' << field.unit << '\n';
        }
    }

    void appendFemLines(std::ostringstream& stream,
                        const std::vector<NamedValue>& fields,
                        int precision)
    {
        for (const NamedValue& field : fields)
        {
            stream << femKey(field.name) << '='
                   << formatNumber(field.value, precision) << ' '
                   << field.unit << '\n';
        }
    }
}

namespace SectionExport
{
    std::string sectionDisplayName(SectionType type)
    {
        return toString(type);
    }

    std::string formatPropertiesReport(const SectionInput& input,
                                       const SectionProperties& properties,
                                       const ExportOptions& options)
    {
        std::ostringstream stream;
        stream << "Beam Element Section Property Tool\n";
        stream << "Section Properties Report\n";
        stream << "Section Type: " << sectionDisplayName(input.type) << "\n\n";

        stream << "Input Parameters\n";
        appendNamedValueBlock(stream, inputFields(input), options.precision, "- ");
        stream << '\n';

        stream << "Calculated Properties\n";
        appendNamedValueBlock(stream, propertyFields(properties), options.precision, "- ");

        return stream.str();
    }

    std::string formatFemCard(const SectionInput& input,
                              const SectionProperties& properties,
                              const ExportOptions& options)
    {
        std::ostringstream stream;
        stream << "# Beam Element Section Definition Card\n";
        stream << "SECTION_TYPE=" << sectionDisplayName(input.type) << '\n';
        stream << '\n';
        stream << "# INPUT_PARAMETERS\n";
        appendFemLines(stream, inputFields(input), options.precision);
        stream << '\n';
        stream << "# CALCULATED_PROPERTIES\n";
        appendFemLines(stream, propertyFields(properties), options.precision);

        return stream.str();
    }
}
