#pragma once

#include "CYGeneric.h"
#include "MeshBuilder.h"
#include "../OldFormat/OldFormatExtractor.h"

class CYPlat : virtual public CYGeneric
{
public:
	// Create from OldFormat/Regex loader
	CYPlat(std::smatch& matchGroups);

	virtual void createMesh(const WorldTextures& wTex);

	virtual Mesh& getMesh();
	virtual MinBoundingBox& getAABB();

	// For serializing
	template <typename Archive>
	void serialize(Archive& archive)
	{
		archive(floor, position, material, height);
	}

	// Return property data
	virtual u8 getLevel() { return floor; };

protected:
	CYObject type = CYObject::CYPlat;

private:
	Property::Position position;
	Property::Material material;

	u8 floor;
	u8 height;
	u8 size;
};