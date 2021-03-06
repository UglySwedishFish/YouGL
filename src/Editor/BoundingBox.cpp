#include "BoundingBox.h"

#include <iostream>

const bool BoundingBox::checkAABB(const BoundingBox& obj)
{
	return (this->m_vecMax.x > obj.m_vecMin.x &&
		this->m_vecMin.x < obj.m_vecMax.x &&
		this->m_vecMax.y > obj.m_vecMin.y &&
		this->m_vecMin.y < obj.m_vecMax.y &&
		this->m_vecMax.z > obj.m_vecMin.z &&
		this->m_vecMin.z < obj.m_vecMax.z);
}

const bool BoundingBox::checkAABB(const glm::vec3& obj_minv3, const glm::vec3& obj_maxv3)
{
	return (this->m_vecMax.x > obj_minv3.x &&
		this->m_vecMin.x < obj_maxv3.x &&
		this->m_vecMax.y > obj_minv3.y &&
		this->m_vecMin.y < obj_maxv3.y &&
		this->m_vecMax.z > obj_minv3.z &&
		this->m_vecMin.z < obj_maxv3.z);
}

const bool BoundingBox::checkAABB(const glm::vec3 & point)
{
	return (point.x > m_vecMin.x &&
		point.x < m_vecMax.x &&
		point.y > m_vecMin.y &&
		point.y < m_vecMax.y &&
		point.z > m_vecMin.z &&
		point.z < m_vecMax.z);
}

const std::optional<float> BoundingBox::checkRayCast(const MouseRay::Ray & ray, float t0, float t1)
{
	// ??? // Not sure why it requires to be changed to Editor coordinates
	// Need to be more careful with how coordinates are treated in this game
	glm::vec3 min = m_vecMin / WORLD_SIZE;
	glm::vec3 max = m_vecMax / WORLD_SIZE;

	// Using an improved version Smits' algorithm as shown in the reference
	// Ref: https://people.csail.mit.edu/amy/papers/box-jgt.pdf
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	// Initialize tmin/max (x axis) & tymin/max (y axis)
	// The improved variant opts for 1 divide & 2 multiplication functions 
	float divx = 1 / ray.direction.x;
	if (divx >= 0) {
		tmin = (min.x - ray.origin.x) * divx;
		tmax = (max.x - ray.origin.x) * divx;
	}
	else {
		tmin = (max.x - ray.origin.x) * divx;
		tmax = (min.x - ray.origin.x) * divx;
	}

	// Y -coord
	float divy = 1 / ray.direction.y;
	if (divy >= 0) {
		tymin = (min.y - ray.origin.y) * divy;
		tymax = (max.y - ray.origin.y) * divy;
	}
	else {
		tymin = (max.y - ray.origin.y) * divy;
		tymax = (min.y - ray.origin.y) * divy;
	}

	if ((tmin > tymax) || (tymin > tmax))
		return std::nullopt;

	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	// Z-coord
	float divz = 1 / ray.direction.z;
	if (divz >= 0) {
		tzmin = (min.z - ray.origin.z) * divz;
		tzmax = (max.z - ray.origin.z) * divz;
	}
	else {
		tzmin = (max.z - ray.origin.z) * divz;
		tzmax = (min.z - ray.origin.z) * divz;
	}

	if ((tmin > tzmax) || (tzmin > tmax))
		return std::nullopt;

	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	//std::cout << "tmin: " << tmin << ", tmax: " << tmax << "\n";
	if ((tmin < t1) && (tmax > t0))
		return std::optional<float>{(tmin > 0) ? tmin : 0.f};
}

const glm::vec3& BoundingBox::getVecMin()
{
	return m_vecMin;
}

const glm::vec3& BoundingBox::getVecMax()
{
	return m_vecMax;
}


BoundingBox::BoundingBox(glm::vec3 min, glm::vec3 max)
	: m_vecMin(min)
	, m_vecMax(max)
{
	// If rendering the boxes then create a model for it
	if (RENDER_BOUNDING_BOX)
	{
		// TODO: Create a namespace for quickly creating primitive shapes DONE
		m_staticBox = Primitives::createCube(m_vecMin / WORLD_SIZE, m_vecMax / WORLD_SIZE, GL_LINES);
	}
}

void BoundingBox::render(Renderer& renderer)
{
	renderer.draw(m_staticBox);
}