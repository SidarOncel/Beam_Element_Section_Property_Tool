#include "SectionProjectIO.hpp"

#include <cctype>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    struct ParameterBinding
    {
        std::string key;
        double SectionInput::* member = nullptr;
    };

    std::string formatNumber(double value, int precision)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(precision) << value;
        return stream.str();
    }

    std::string trim(const std::string& value)
    {
        std::size_t first = 0;
        while (first < value.size() &&
               std::isspace(static_cast<unsigned char>(value[first])))
        {
            ++first;
        }

        std::size_t last = value.size();
        while (last > first &&
               std::isspace(static_cast<unsigned char>(value[last - 1])))
        {
            --last;
        }

        return value.substr(first, last - first);
    }

    std::string projectTypeName(SectionType type)
    {
        switch (type)
        {
        case SectionType::HSection:
            return "H_SECTION";
        case SectionType::BoxSection:
            return "BOX_SECTION";
        case SectionType::PipeSection:
            return "PIPE_SECTION";
        case SectionType::GirderSection:
            return "GIRDER_SECTION";
        }

        return "";
    }

    bool parseProjectType(const std::string& value, SectionType& type)
    {
        if (value == "H_SECTION")
        {
            type = SectionType::HSection;
            return true;
        }
        if (value == "BOX_SECTION")
        {
            type = SectionType::BoxSection;
            return true;
        }
        if (value == "PIPE_SECTION")
        {
            type = SectionType::PipeSection;
            return true;
        }
        if (value == "GIRDER_SECTION")
        {
            type = SectionType::GirderSection;
            return true;
        }

        return false;
    }

    std::vector<ParameterBinding> parameterBindings(SectionType type)
    {
        switch (type)
        {
        case SectionType::HSection:
            return {
                {"width", &SectionInput::width},
                {"height", &SectionInput::height},
                {"flangeThickness", &SectionInput::flangeThickness},
                {"webThickness", &SectionInput::webThickness}
            };
        case SectionType::BoxSection:
            return {
                {"width", &SectionInput::width},
                {"height", &SectionInput::height},
                {"topFlangeThickness", &SectionInput::topFlangeThickness},
                {"bottomFlangeThickness", &SectionInput::bottomFlangeThickness},
                {"leftWebThickness", &SectionInput::leftWebThickness},
                {"rightWebThickness", &SectionInput::rightWebThickness}
            };
        case SectionType::PipeSection:
            return {
                {"diameter", &SectionInput::diameter},
                {"wallThickness", &SectionInput::wallThickness}
            };
        case SectionType::GirderSection:
            return {
                {"A", &SectionInput::A},
                {"B", &SectionInput::B},
                {"G", &SectionInput::G},
                {"D", &SectionInput::D},
                {"e", &SectionInput::e},
                {"f", &SectionInput::f},
                {"H", &SectionInput::H},
                {"W", &SectionInput::W},
                {"M", &SectionInput::M},
                {"N", &SectionInput::N},
                {"p", &SectionInput::p},
                {"s", &SectionInput::s},
                {"t", &SectionInput::t},
                {"u", &SectionInput::u},
                {"M1", &SectionInput::M1},
                {"k", &SectionInput::k},
                {"k1", &SectionInput::k1},
                {"h", &SectionInput::h},
                {"h1", &SectionInput::h1}
            };
        }

        return {};
    }

    bool parseDouble(const std::string& text, double& value)
    {
        try
        {
            std::size_t parsed = 0;
            value = std::stod(text, &parsed);
            return parsed == text.size();
        }
        catch (...)
        {
            return false;
        }
    }

    bool readProjectFile(const std::string& filePath,
                         std::map<std::string, std::string>& sectionValues,
                         std::map<std::string, std::string>& parameterValues)
    {
        std::ifstream file(filePath);
        if (!file)
        {
            return false;
        }

        enum class Block
        {
            None,
            Section,
            Parameters
        };

        Block block = Block::None;
        std::string line;
        while (std::getline(file, line))
        {
            line = trim(line);
            if (line.empty())
            {
                continue;
            }

            if (line == "[Section]")
            {
                block = Block::Section;
                continue;
            }
            if (line == "[Parameters]")
            {
                block = Block::Parameters;
                continue;
            }

            const std::size_t separator = line.find('=');
            if (separator == std::string::npos || block == Block::None)
            {
                return false;
            }

            const std::string key = trim(line.substr(0, separator));
            const std::string value = trim(line.substr(separator + 1));
            if (key.empty() || value.empty())
            {
                return false;
            }

            if (block == Block::Section)
            {
                sectionValues[key] = value;
            }
            else
            {
                parameterValues[key] = value;
            }
        }

        return true;
    }
}

namespace SectionProjectIO
{
    bool saveSectionProject(const SectionInput& input,
                            const std::string& filePath,
                            const ExportOptions& options)
    {
        try
        {
            std::ofstream file(filePath);
            if (!file)
            {
                return false;
            }

            file << "[Section]\n";
            file << "type=" << projectTypeName(input.type) << '\n';
            file << "unit=mm\n";
            file << "precision=" << options.precision << "\n\n";

            file << "[Parameters]\n";
            for (const ParameterBinding& binding : parameterBindings(input.type))
            {
                file << binding.key << '='
                     << formatNumber(input.*(binding.member), options.precision) << '\n';
            }

            return static_cast<bool>(file);
        }
        catch (...)
        {
            return false;
        }
    }

    bool loadSectionProject(const std::string& filePath,
                            SectionInput& input)
    {
        try
        {
            std::map<std::string, std::string> sectionValues;
            std::map<std::string, std::string> parameterValues;
            if (!readProjectFile(filePath, sectionValues, parameterValues))
            {
                return false;
            }

            const auto typeIt = sectionValues.find("type");
            if (typeIt == sectionValues.end())
            {
                return false;
            }

            SectionType type = SectionType::HSection;
            if (!parseProjectType(typeIt->second, type))
            {
                return false;
            }

            const auto unitIt = sectionValues.find("unit");
            if (unitIt != sectionValues.end() && unitIt->second != "mm")
            {
                return false;
            }

            SectionInput loaded;
            loaded.type = type;
            for (const ParameterBinding& binding : parameterBindings(type))
            {
                const auto valueIt = parameterValues.find(binding.key);
                if (valueIt == parameterValues.end())
                {
                    return false;
                }

                double value = 0.0;
                if (!parseDouble(valueIt->second, value))
                {
                    return false;
                }

                loaded.*(binding.member) = value;
            }

            input = loaded;
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
}
