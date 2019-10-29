#include "pch.h"
#include <iostream>
#include "MeshFileConverter_OBJ_STL.h"

MeshFileConverter_OBJ_STL* MeshFileConverter_OBJ_STL::g_Singleton = nullptr;

std::string MeshFileConverter_OBJ_STL::Convert(const std::string& filepath, const std::string& stats_str, const std::string& conversionparams_str)
{
	std::string errors;

	ConversionParams_OBJ_STL params;
	errors = params.Parse(conversionparams_str);
	if (errors.length() > 0) return errors;

	ConversionStats_OBJ_STL stats;
	errors = stats.Parse(stats_str);
	if (errors.length() > 0) return errors;

	ConvertCore* cc = new ConvertCore();

	if (cc->readIn(errors, filepath.c_str()) == true)
	{
		cc->writeOut(errors, std::string(filepath + GetDestExtension()).c_str(), params, stats);
	}

	if (stats.m_Result.length() > 0)
	{
		std::ofstream statfile(filepath + GetDestExtension() + ".stats");
		statfile << stats.m_Result;
	}

	delete cc;

	return errors;
}

std::string MeshFileConverter_OBJ_STL::ConversionStats_OBJ_STL::Parse(const std::string& str)
{
	m_Stats.clear();

	std::istringstream iss(str);

	std::string p;
	iss >> p;

	do
	{
		if (p == "CALC_SURFACE")
		{
			sStat stat;
			stat.m_Stat = CONVERSIONSTAT_CALC_SURFACE;
			m_Stats.push_back(stat);
		}
		else
		if (p == "CALC_VOLUME")
		{
			sStat stat;
			stat.m_Stat = CONVERSIONSTAT_CALC_VOLUME;
			m_Stats.push_back(stat);
		}
		else
		if (p == "CALC_3DPOINT_INSIDE")
		{
			iss >> p;
			sStat stat;
			stat.m_Stat = CONVERSIONSTAT_CALC_3DPOINT_INSIDE;
			stat.m_StatHint = p;
			m_Stats.push_back(stat);
		}
		else
		{
			return "Invalid syntax for ConversionStats_OBJ_STL: <CALC_SURFACE | CALC_VOLUME | CALC_3DPOINT_INSIDE [x,y,z]>";
		}

		p = "";
		iss >> p;

	} while (iss.eof() == false || p.length() > 0);

	return "";
}

std::string MeshFileConverter_OBJ_STL::ConversionParams_OBJ_STL::Parse(const std::string& str)
{
	m_Params.clear();

	std::istringstream iss(str);

	std::string p;
	iss >> p;
	do
	{
		if (p == "TRANSLATE")
		{
			iss >> p;
			sParam stat;
			stat.m_Param = CONVERSIONPARAM_TRANSFORM_TRANSLATE_XYZ;
			stat.m_ParamValaue = p;
			
			m_Params.push_back(stat);
		}
		else
		if (p == "ROTATE")
		{
			iss >> p;
			sParam stat;
			stat.m_Param = CONVERSIONPARAM_TRANSFORM_ROTATE_XYZ;
			stat.m_ParamValaue = p;
			m_Params.push_back(stat);
		}
		else
		if (p == "SCALE")
		{
			iss >> p;
			sParam stat;
			stat.m_Param = CONVERSIONPARAM_TRANSFORM_SCALE_XYZ;
			stat.m_ParamValaue = p;
			m_Params.push_back(stat);
		}
		else
		{
			return "Invalid syntax for ConversionStats_OBJ_STL: <TRANSLATE | ROTATE | SCALE>";
		}

		p = "";
		iss >> p;
	} while (iss.eof() == false || p.length() > 0);

	return "";
}


bool MeshFileConverter_OBJ_STL::ConvertCore::readIn(std::string& errors, const char* fPath)
{
	FILE* file = fopen(fPath, "r");
	if (file == NULL)
	{
		errors += "Error opening file !" + std::string(fPath) + "\n";
		return false;
	}

	while (1)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			fclose(file);
			break; // EOF = End Of File. Quit the loop.
		}


		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);
		}
		else
		if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec3 uv;
			fscanf(file, "%f %f %f\n", &uv.x, &uv.y, &uv.z);
			uvs.push_back(uv);
		}
		else
		if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
		else
		if (strcmp(lineHeader, "f") == 0)
		{

			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches;
		
			if (uvs.size() == 0)
			{
				if (normals.size() == 0)
				{
					matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
					vertexIndices.push_back(glm::vec3(vertexIndex[0], vertexIndex[1], vertexIndex[2]));
				}
				else
				{
					matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
					vertexIndices.push_back(glm::vec3(vertexIndex[0], vertexIndex[1], vertexIndex[2]));
					normalIndices.push_back(glm::vec3(normalIndex[0], normalIndex[1], normalIndex[2]));
				}
			}
			else
			if (normals.size() == 0)
			{
				matches = fscanf(file, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
				vertexIndices.push_back(glm::vec3(vertexIndex[0], vertexIndex[1], vertexIndex[2]));
				uvIndices.push_back(glm::vec3(uvIndex[0], uvIndex[1], uvIndex[2]));
			}
			else
			{
				matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				vertexIndices.push_back(glm::vec3(vertexIndex[0], vertexIndex[1], vertexIndex[2]));
				uvIndices.push_back(glm::vec3(uvIndex[0], uvIndex[1], uvIndex[2]));
				normalIndices.push_back(glm::vec3(normalIndex[0], normalIndex[1], normalIndex[2]));
			}

		}
	}

	return true;
}

bool MeshFileConverter_OBJ_STL::ConvertCore::is_ccw(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	// calculate three axes
	glm::vec3 axis_x = glm::normalize(b - a);
	glm::vec3 axis_y = glm::normalize(c - a);
	glm::vec3 axis_z = glm::cross(axis_x, axis_y);

	// construct a transform matrix from our axes
	glm::mat3x3 object_transform;
	object_transform[0] = axis_x;
	object_transform[1] = axis_y;
	object_transform[2] = axis_z;

	// invert the matrix
	glm::mat3x3 object_to_world_transform = glm::inverse(object_transform);

	// transform the outward normal using the matrix
	glm::vec3 normal = object_to_world_transform * axis_z;

	// check winding
	if (normal.z > 0.f)
	{
		// Counter-clockwise winding
		return true;
	}
	else
	{
		// Clockwise winding
		return false;
	}
}

bool MeshFileConverter_OBJ_STL::ConvertCore::writeOut(std::string& errors, const char* fPath, const ConversionParams_OBJ_STL& params, ConversionStats_OBJ_STL& stats)
{
	FILE* file = fopen(fPath, "w");
	if (file == NULL)
	{
		errors += "Error writing file !" + std::string(fPath) + "\n";
		return false;
	}

	int size = vertexIndices.size();
	const char header[80] = "header";
	short zero = 0;
	printf("%s\n", header);
	printf("%d\n", size);
	fwrite(&header, 80, 1, file);
	fwrite(&size, sizeof(int), 1, file);

	for (size_t ci = 0; ci < params.m_Params.size(); ci++)
	{
		ConversionParams::sParam param = params.m_Params[ci];
		if (param.m_Param == ConversionParams::CONVERSIONPARAM_TRANSFORM_TRANSLATE_XYZ)
		{
			glm::vec3 tmpVec;
			std::cout << "Give the offset of the translation on X axis (floating point number): ";
			std::cin >> tmpVec.x;
			std::cout << "Give the offset of the translation  on Y axis (floating point number): ";
			std::cin >> tmpVec.y;
			std::cout << "Give the offset of the translation  on Z axis (floating point number): ";
			std::cin >> tmpVec.z;
			translation(tmpVec);
		}
		else
		if (param.m_Param == ConversionParams::CONVERSIONPARAM_TRANSFORM_SCALE_XYZ)
		{
			glm::vec3 tmpVec;
			std::cout << "Give the size of the scale on X axis (floating point number): ";
			std::cin >> tmpVec.x;
			std::cout << "Give the size of the scale on Y axis (floating point number): ";
			std::cin >> tmpVec.y;
			std::cout << "Give the size of the scale on Z axis (floating point number): ";
			std::cin >> tmpVec.z;
			scale(tmpVec);
		}
		else
		if (param.m_Param == ConversionParams::CONVERSIONPARAM_TRANSFORM_ROTATE_XYZ)
		{
			glm::vec3 tmpVec;
			std::cout << "Give the angle of the rotation on X axis (floating point number): ";
			std::cin >> tmpVec.x;
			std::cout << "Give the angle of the rotation on Y axis (floating point number): ";
			std::cin >> tmpVec.y;
			std::cout << "Give the angle of the rotation on Z axis (floating point number): ";
			std::cin >> tmpVec.z;
			rotate(tmpVec.x, tmpVec.y, tmpVec.z);
		}
		std::cout << std::endl << "Transformations are successfully performed." << std::endl;
	}

	for (size_t ci = 0; ci < stats.m_Stats.size(); ci++)
	{
		ConversionStats::sStat stat = stats.m_Stats[ci];
		if (stat.m_Stat == ConversionStats::CONVERSIONSTAT_CALC_VOLUME)
		{
			if (stats.m_Result.length() > 0) stats.m_Result += "\r\n";
			stats.m_Result += "VOLUME=";
			float volume = volumeOfMesh();
			std::stringstream ss;
			ss << volume;
			stats.m_Result += ss.str();
		}
		else
		if (stat.m_Stat == ConversionStats::CONVERSIONSTAT_CALC_SURFACE)
		{
			if (stats.m_Result.length() > 0) stats.m_Result += "\r\n";
			stats.m_Result += "SURFACE=";

			float surface = calculateSurfaceArea();
			std::stringstream ss;
			ss << surface;
			stats.m_Result += ss.str();
		}
		else
		if (stat.m_Stat == ConversionStats::CONVERSIONSTAT_CALC_3DPOINT_INSIDE)
		{
			glm::vec3 p;
			std::cout << "Give the point: ";
			std::cin >> p.x >> p.y >> p.z;
			bool isInside = isPointInsideMesh(p);
			if (stats.m_Result.length() > 0) stats.m_Result += "\r\n";
			stats.m_Result += "IS POINT INSIDE:";
			std::string res = "FALSE";
			if (isInside == true) res = "TRUE";
			std::stringstream ss;
			ss << p.x;
			ss << " ";
			ss << p.y;
			ss << " ";
			ss << p.z;
			ss << " ";
			ss << res;
			stats.m_Result += ss.str();
		}
		std::cout << std::endl << "Stats is successfully written out to .stat file" <<  std::endl;
	}
	
	// Write out
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		glm::vec3 p1 = vertices[(unsigned int)vertexIndices[i].x - 1];
		glm::vec3 p2 = vertices[(unsigned int)vertexIndices[i].y - 1];
		glm::vec3 p3 = vertices[(unsigned int)vertexIndices[i].z - 1];

		glm::vec3 vec1 = p1 - p2;
		glm::vec3 vec2 = p1 - p3;

		glm::vec3 tmpNormal = glm::cross(vec1, vec2);
		tmpNormal = glm::normalize(tmpNormal);
		// Normal
		fwrite(&tmpNormal, sizeof(tmpNormal), 1, file);
		// 3 points
		fwrite(&p1, sizeof(p1), 1, file);
		fwrite(&p2, sizeof(p2), 1, file);
		fwrite(&p3, sizeof(p3), 1, file);

		fwrite(&zero, sizeof(short), 1, file);
		
	}
	fclose(file);
	clear();

	return true;
}