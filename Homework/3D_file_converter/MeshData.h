#pragma once
#include <vector>
#include <glm.hpp>

class MeshData
{
public:
	std::vector<glm::vec3>	vertexIndices;
	std::vector<glm::vec3>	uvIndices;
	std::vector<glm::vec3>	normalIndices;
	std::vector<glm::vec3>	vertices;
	std::vector<glm::vec2>	uvs;
	std::vector<glm::vec3>	normals;

	std::vector<glm::mat4>	transformationMatrices;
	std::vector<glm::vec3>	boundingboxPoints;

	void	clear();
	void	getBoundingbox();
	void	multiplyVerticesWithMx();
	void	scale(glm::vec3 size);
	void	rotate(float angleX, float angleY, float angleZ);
	void	translation(glm::vec3 values);
	bool	isPointInsideMesh(glm::vec3 point);
	float	signedVolumeOfTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
	float	volumeOfMesh();
	float	calculateSurfaceArea();
};