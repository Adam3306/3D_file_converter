#include <algorithm>
#include "pch.h"
#include "MeshData.h"

void MeshData::clear()
{
	vertexIndices.clear();
	uvIndices.clear();
	normalIndices.clear();
	vertices.clear();
	uvs.clear();
	normals.clear();
	transformationMatrices.clear();
}

float calcAreaOfTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	float ax = p2.x - p1.x;
	float ay = p2.y - p1.y;
	float az = p2.z - p1.z;
	float bx = p3.x - p1.x;
	float by = p3.y - p1.y;
	float bz = p3.z - p1.z;

	float cx = ay * bz - az * by;
	float cy = az * bx - ax * bz;
	float cz = ax * by - ay * bx;

	return (float)0.5 * sqrt(cx*cx + cy * cy + cz * cz);
}

bool isPointInsideTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	// Calculate area of triangle ABC 
	float A = calcAreaOfTriangle(a, b, c);

	// Calculate area of triangle PBC
	float A1 = calcAreaOfTriangle(p, b, c);

	// Calculate area of triangle PAC
	float A2 = calcAreaOfTriangle(p, a , c);

	// Calculate area of triangle PAB
	float A3 = calcAreaOfTriangle(p, a, b);

	// Check if sum of A1, A2 and A3 is same as A
	return (A == A1 + A2 + A3);
}

void MeshData::getBoundingbox()
{
	float minX = vertices[0].x; float  maxX = vertices[0].x;
	float minY = vertices[0].y; float  maxY = vertices[0].y;
	float minZ = vertices[0].z; float  maxZ = vertices[0].z;

	for (unsigned int i = 1; i < vertices.size(); i++)
	{
		if (vertices[i].x < minX) minX = vertices[i].x;
		if (vertices[i].y < minY) minY = vertices[i].y;
		if (vertices[i].z < minZ) minZ = vertices[i].z;

		if (vertices[i].x > maxX) maxX = vertices[i].x;
		if (vertices[i].y > maxY) maxY = vertices[i].y;
		if (vertices[i].z > maxZ) maxZ = vertices[i].z;
	}
	
	boundingboxPoints.push_back(glm::vec3(minX, minY, minZ));
	boundingboxPoints.push_back(glm::vec3(maxX, maxY, maxZ));
}

bool MeshData::isPointInsideMesh(glm::vec3 point)
{
	getBoundingbox();
	// first check if point is inside the bounding box of the shape
	// this can be huge speedup
	if (boundingboxPoints[0].x < point.x &&  point.x < boundingboxPoints[1].x &&
		boundingboxPoints[0].y < point.y &&  point.y < boundingboxPoints[1].y &&
		boundingboxPoints[0].z < point.z &&  point.z < boundingboxPoints[1].z)
	{
		// if the point is inside the bounding box, lets get a point outside of the bounding box
		// and check if the line from that point to the given point passes through the surfaces of the shape
		// if it passes through an odd number of surfaces, the given point is inside the shape
		glm::vec3 pointOutside(boundingboxPoints[1].x + 1000, boundingboxPoints[1].y + 1000, boundingboxPoints[1].z + 1000);
		glm::vec3 vec = pointOutside - point;
		int count = 0;
		std::vector<glm::vec3> intersectionPoints;
		for (unsigned int i = 0; i < vertexIndices.size(); i++)
		{
			glm::vec3 p1 = vertices[(unsigned int)vertexIndices[i].x - 1];
			glm::vec3 p2 = vertices[(unsigned int)vertexIndices[i].y - 1];
			glm::vec3 p3 = vertices[(unsigned int)vertexIndices[i].z - 1];

			glm::vec3 vec1 = p1 - p2;
			glm::vec3 vec2 = p1 - p3;

			glm::vec3 tmpNormal = glm::cross(vec1, vec2);
			float res = glm::dot(tmpNormal, vec);
			if (res == 0)
			{
				// that means the vectors are parallel
				// bc the angle beetween the 2 vectors is 90 degrees
				return false;
			}
			else
			{
				float s = glm::dot(tmpNormal, p1 - point) / res;
				glm::vec3 intersectionP = point + s * vec;
				bool contains = std::find(intersectionPoints.begin(), intersectionPoints.end(), intersectionP) != intersectionPoints.end();
				if (0.0f < s && isPointInsideTriangle(intersectionP, p1, p2, p3) && contains == false)
				{
					intersectionPoints.push_back(intersectionP);
					count++;
				}
			}
		}
		return count % 2 == 1;
	}
	else
	{
		return false;
	}
}

void MeshData::multiplyVerticesWithMx()
{
	// if we multiply the matrices first and then multiply the vertices is faster than multiply each matrices with the vertices
	glm::mat4 resMat(1.0f);
	for (unsigned int i = 0; i < transformationMatrices.size(); i++)
	{
		resMat *= transformationMatrices[i];
	}

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		glm::vec3* p1 = &vertices[i];
		glm::vec4 v1(p1->x, p1->y, p1->z, 1);
		glm::vec4 res = v1 * resMat;
		p1->x = res.x / res.w;
		p1->y = res.y / res.w;
		p1->z = res.z / res.w;
	}
}

void MeshData::scale(glm::vec3 size)
{
	float tmpmx[16] =
	{
		size.x,		0,			0,			0,
		0,			size.y,		0,			0,
		0,			0,			size.z,		0,
		0,			0,			0,			1
	};

	glm::mat4 scale_mx;
	memcpy(&scale_mx, tmpmx, sizeof(tmpmx));
	transformationMatrices.push_back(scale_mx);
}

void MeshData::rotate(float angleX, float angleY, float angleZ)
{
	if (angleX != 0)
	{
		float rotate_by_X_axis_tmpmx[16] =
		{
			0,		0,				0,						0,
			0,		cos(angleX),	-sin(angleX),			0,
			0,		sin(angleX),	cos(angleX),			0,
			0,			0,			0,						1
		};
		glm::mat4 rot_mx;
		memcpy(&rot_mx, rotate_by_X_axis_tmpmx, sizeof(rotate_by_X_axis_tmpmx));
		transformationMatrices.push_back(rot_mx);
	}

	if (angleY != 0)
	{
		float rotate_by_Y_axis_tmpmx[16] =
		{
			cos(angleY),  0,	sin(angleY),	0,
			0,			  1,	0,				0,
			-sin(angleY), 0,	cos(angleY),	0,
			0,			  0,	0,				1
		};
		glm::mat4 rot_mx;
		memcpy(&rot_mx, rotate_by_Y_axis_tmpmx, sizeof(rotate_by_Y_axis_tmpmx));
		transformationMatrices.push_back(rot_mx);
	}

	if (angleZ != 0)
	{
		float rotate_by_Z_axis_tmpmx[16] =
		{
			cos(angleZ),	-sin(angleZ),	0,	0,
			sin(angleZ),	cos(angleZ),	0,	0,
			0,				0,				1,	0,
			0,				0,				0,	1
		};

		glm::mat4 rot_mx;
		memcpy(&rot_mx, rotate_by_Z_axis_tmpmx, sizeof(rotate_by_Z_axis_tmpmx));
		transformationMatrices.push_back(rot_mx);
	}
}

void MeshData::translation(glm::vec3 values)
{
	float tmpmx[16] =
	{
		1,		0,			0,			values.x,
		0,		1,			0,			values.y,
		0,		0,			1,			values.z,
		0,		0,			0,			1
	};

	glm::mat4 translation_mx;
	memcpy(&translation_mx, tmpmx, sizeof(tmpmx));
	transformationMatrices.push_back(translation_mx);
}

float MeshData::signedVolumeOfTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	float v321 = p3.x * p2.y * p1.z;
	float v231 = p2.x * p3.y * p1.z;
	float v312 = p3.x * p1.y * p2.z;
	float v132 = p1.x * p3.y * p2.z;
	float v213 = p2.x * p1.y * p3.z;
	float v123 = p1.x * p2.y * p3.z;
	return (float)(1.0 / 6.0) * (-v321 + v231 + v312 - v132 - v213 + v123);
}

float MeshData::volumeOfMesh()
{
	float volume = 0.0;

	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		glm::vec3 p1 = vertices[(unsigned int)vertexIndices[i].x - 1];
		glm::vec3 p2 = vertices[(unsigned int)vertexIndices[i].y - 1];
		glm::vec3 p3 = vertices[(unsigned int)vertexIndices[i].z - 1];

		volume += signedVolumeOfTriangle(p1, p2, p3);
	}

	return abs(volume);
}

float MeshData::calculateSurfaceArea()
{
	double sum = 0.0;

	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		glm::vec3 p1 = vertices[(unsigned int)vertexIndices[i].x - 1];
		glm::vec3 p2 = vertices[(unsigned int)vertexIndices[i].y - 1];
		glm::vec3 p3 = vertices[(unsigned int)vertexIndices[i].z - 1];

		sum += calcAreaOfTriangle(p1, p2, p3);
	}

	return (float)(sum);
}