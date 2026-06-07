#pragma once

#include "PropertyCalculator.hpp"

#include <string>

struct ExportOptions
{
    int precision = 2;
};

namespace SectionExport
{
    std::string sectionDisplayName(SectionType type);

    std::string formatPropertiesReport(const SectionInput& input,
                                       const SectionProperties& properties,
                                       const ExportOptions& options = {});

    std::string formatFemCard(const SectionInput& input,
                              const SectionProperties& properties,
                              const ExportOptions& options = {});

    bool writePropertiesReportToFile(const SectionInput& input,
                                     const SectionProperties& properties,
                                     const std::string& filePath,
                                     const ExportOptions& options = {});

    bool writeFemCardToFile(const SectionInput& input,
                            const SectionProperties& properties,
                            const std::string& filePath,
                            const ExportOptions& options = {});
}
