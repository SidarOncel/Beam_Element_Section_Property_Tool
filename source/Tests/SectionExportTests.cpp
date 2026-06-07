#include "../Core/PropertyCalculator.hpp"
#include "../Core/SectionExport.hpp"
#include "../Core/SectionProjectIO.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

namespace
{
    int failedTests = 0;
    int passedTests = 0;

    void expectTrue(bool condition, const std::string& testName)
    {
        if (condition)
        {
            std::cout << "[PASS] " << testName << '\n';
            ++passedTests;
        }
        else
        {
            std::cout << "[FAIL] " << testName << '\n';
            ++failedTests;
        }
    }

    bool contains(const std::string& text, const std::string& expected)
    {
        return text.find(expected) != std::string::npos;
    }

    bool near(double lhs, double rhs)
    {
        return std::abs(lhs - rhs) < 1e-9;
    }

    SectionInput hSectionInput()
    {
        SectionInput input;
        input.type = SectionType::HSection;
        input.width = 100.0;
        input.height = 250.0;
        input.flangeThickness = 20.0;
        input.webThickness = 12.0;
        return input;
    }

    SectionInput boxSectionInput()
    {
        SectionInput input;
        input.type = SectionType::BoxSection;
        input.width = 1320.0;
        input.height = 2600.0;
        input.topFlangeThickness = 40.0;
        input.bottomFlangeThickness = 22.0;
        input.leftWebThickness = 16.0;
        input.rightWebThickness = 16.0;
        return input;
    }

    void writeInvalidProjectFile(const std::string& path)
    {
        std::ofstream file(path);
        file << "[Section]\n";
        file << "type=NOT_A_SECTION\n";
        file << "unit=mm\n";
        file << "precision=2\n\n";
        file << "[Parameters]\n";
        file << "width=abc\n";
    }

    void testExportFormatting()
    {
        const SectionInput input = hSectionInput();
        const SectionProperties properties =
            PropertyCalculator::calculateHSectionProperties(input);

        const std::string report =
            SectionExport::formatPropertiesReport(input, properties);
        expectTrue(contains(report, "Section Type: H Section"),
                   "Report contains section display name");
        expectTrue(contains(report, "- Area: 6520.00 mm^2"),
                   "Report contains area with two decimals and units");
        expectTrue(contains(report, "- Jz: 3363573.33 mm^4"),
                   "Report contains inertia with mm^4 units");

        const std::string femCard = SectionExport::formatFemCard(input, properties);
        expectTrue(contains(femCard, "SECTION_TYPE=H Section"),
                   "FEM card contains deterministic section type line");
        expectTrue(contains(femCard, "FLANGE_THICKNESS=20.00 mm"),
                   "FEM card contains stable parameter key");
        expectTrue(contains(femCard, "AREA=6520.00 mm^2"),
                   "FEM card contains property line with units");
    }

    void testFileWriting()
    {
        const SectionInput input = hSectionInput();
        const SectionProperties properties =
            PropertyCalculator::calculateHSectionProperties(input);

        expectTrue(SectionExport::writePropertiesReportToFile(
                       input, properties, "/tmp/feature6_h_report.txt"),
                   "Report file writes successfully");
        expectTrue(SectionExport::writeFemCardToFile(
                       input, properties, "/tmp/feature6_h_fem_card.txt"),
                   "FEM card file writes successfully");
        expectTrue(!SectionExport::writePropertiesReportToFile(
                       input, properties, "/tmp/missing_folder/feature6_report.txt"),
                   "Invalid report path fails cleanly");
    }

    void testProjectRoundTrip()
    {
        const SectionInput hInput = hSectionInput();
        const std::string hPath = "/tmp/feature6_h_project.txt";
        SectionInput loadedH;
        expectTrue(SectionProjectIO::saveSectionProject(hInput, hPath),
                   "H section project saves successfully");
        expectTrue(SectionProjectIO::loadSectionProject(hPath, loadedH),
                   "H section project loads successfully");
        expectTrue(loadedH.type == SectionType::HSection &&
                       near(loadedH.width, hInput.width) &&
                       near(loadedH.height, hInput.height) &&
                       near(loadedH.flangeThickness, hInput.flangeThickness) &&
                       near(loadedH.webThickness, hInput.webThickness),
                   "H section project roundtrip preserves parameters");

        const SectionInput boxInput = boxSectionInput();
        const std::string boxPath = "/tmp/feature6_box_project.txt";
        SectionInput loadedBox;
        expectTrue(SectionProjectIO::saveSectionProject(boxInput, boxPath),
                   "Box section project saves successfully");
        expectTrue(SectionProjectIO::loadSectionProject(boxPath, loadedBox),
                   "Box section project loads successfully");
        expectTrue(loadedBox.type == SectionType::BoxSection &&
                       near(loadedBox.width, boxInput.width) &&
                       near(loadedBox.height, boxInput.height) &&
                       near(loadedBox.topFlangeThickness, boxInput.topFlangeThickness) &&
                       near(loadedBox.bottomFlangeThickness, boxInput.bottomFlangeThickness) &&
                       near(loadedBox.leftWebThickness, boxInput.leftWebThickness) &&
                       near(loadedBox.rightWebThickness, boxInput.rightWebThickness),
                   "Box section project roundtrip preserves parameters");
    }

    void testLoadFailures()
    {
        SectionInput input;
        expectTrue(!SectionProjectIO::loadSectionProject(
                       "/tmp/feature6_project_does_not_exist.txt", input),
                   "Missing project file fails cleanly");

        const std::string invalidPath = "/tmp/feature6_invalid_project.txt";
        writeInvalidProjectFile(invalidPath);
        expectTrue(!SectionProjectIO::loadSectionProject(invalidPath, input),
                   "Invalid project file fails cleanly");
    }
}

int main()
{
    testExportFormatting();
    testFileWriting();
    testProjectRoundTrip();
    testLoadFailures();

    std::cout << "========================================\n";
    if (failedTests == 0)
    {
        std::cout << "ALL FEATURE 6 TESTS PASSED (" << passedTests
                  << " assertions)\n";
        return 0;
    }

    std::cout << failedTests << " FEATURE 6 TESTS FAILED\n";
    return 1;
}
