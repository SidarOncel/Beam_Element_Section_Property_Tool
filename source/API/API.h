#pragma once

#include "SectionInput.hpp"
#include "SectionShape.hpp"
#include "PropertyCalculator.hpp"
#include "MeshGenerator.hpp"

// DLL 
#ifdef BEAM_SECTION_API_EXPORTS
  #define BEAM_SECTION_API __declspec(dllexport)
#else
  #define BEAM_SECTION_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  compute specific property
 * @param  input  input parameter
 * @param  out    output property
 * @return 0 success，-1 null，-2 unkown
 */
BEAM_SECTION_API int CalculateSectionProperties(
    const SectionInput* input,
    SectionProperties* out);

/**
 * @brief  generate mesh
 * @param  shape      shape descripition
 * @param  meshSize   target mesh (mm)
 * @param  out        output
 * @return 0 success，-1 fail
 */
BEAM_SECTION_API int GenerateMeshFromShape(
    const SectionShape* shape,
    double meshSize,
    MeshData* out);

#ifdef __cplusplus
}
#endif
