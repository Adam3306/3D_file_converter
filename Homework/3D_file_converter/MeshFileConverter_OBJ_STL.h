#pragma once
#include <sstream>
#include <glm.hpp>
#include <fstream>
#include "MeshFileConverter.h"
#include "MeshData.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// #####
// #####	1. Create a software architecture, that allows you to read, convert, and write 3D mesh file formats.
// #####	The architecture should make it possible to easily extend the implementation with new file formats.
// #####
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// $$$$ SOLUTION
// $$$$ 
// $$$$ class MeshFileConverter_XXX_YYY : public MeshFileConverter
// $$$$ {
// $$$$		// to make it possible to easily extend the implementation with new file formats implement a subclass of MeshFileConverter for XXX and YYY fileformat
// $$$$ };
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MeshFileConverter_OBJ_STL : public MeshFileConverter
{
private:
	static MeshFileConverter_OBJ_STL* g_Singleton;
	MeshFileConverter_OBJ_STL()
	{
	}
public:
	class ConversionStats_OBJ_STL : public ConversionStats
	{
	public:
		virtual std::string Parse(const std::string& str) override;
	};

	class ConversionParams_OBJ_STL : public ConversionParams
	{
	public:
		virtual std::string Parse(const std::string& str) override;
	};

	static MeshFileConverter_OBJ_STL* GetSingleton()
	{
		if (g_Singleton == nullptr) g_Singleton = new MeshFileConverter_OBJ_STL();
		return g_Singleton;
	}

	virtual		std::string GetSrcExtension() {	return ".obj"; }
	virtual		std::string GetDestExtension() { return ".stl"; }
	virtual		std::string Convert(const std::string& filepath, const std::string& stats_str, const std::string& conversionparams_str) override;

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// ##### 
	// ##### 2.Implement a OBJ->STL conversion(only one direction!From OBJ to binary STL).The OBJ file support should be limited to v, vt, vn and f elements.STL export should be binary STL, not the text version.Note that OBJ supports non triangular faces.You can assume that these are always convex planar faces.
	// ##### 
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	class ConvertCore : public MeshData
	{
	public:
		bool	readIn(std::string& errors, const char* fPath);
		bool	is_ccw(glm::vec3 a, glm::vec3 b, glm::vec3 c);
		bool	writeOut(std::string& errors, const char* fPath, const ConversionParams_OBJ_STL& params, ConversionStats_OBJ_STL& stats);
	};
};