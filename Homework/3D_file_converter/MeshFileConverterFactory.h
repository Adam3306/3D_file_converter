#pragma once
#include <string>
#include <fstream>
#include "MeshFileConverter.h"
#include "ExtraValidationLayer.h"

// Write a general 3D file converter, and create a concrete implementation for an OBJ reader and an STL writer.
class MeshFileConverterFactory
{
	std::vector<MeshFileConverter*> m_Converters;
public:
	void ConvertFile(const std::string& src, const std::string& targetextension, const std::string& stats, const std::string& conversionparams);
	void RegisterConverter(MeshFileConverter* converter);
	
};