#pragma once

#include "SectionExport.hpp"

#include <string>

namespace SectionProjectIO
{
    bool saveSectionProject(const SectionInput& input,
                            const std::string& filePath,
                            const ExportOptions& options = {});

    bool loadSectionProject(const std::string& filePath,
                            SectionInput& input);
}
