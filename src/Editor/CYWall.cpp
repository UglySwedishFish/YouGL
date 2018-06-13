#include "CYWall.h"


// WEB PARSER CONSTRUCTOR
CYWall::CYWall(const std::smatch& matchGroups)
{
    std::ssub_match subMatch;

    // Start Position
    subMatch = matchGroups[1];
    this->m_displacementPosition = sf::Vector2f(std::stof(subMatch.str()), 0);
    subMatch = matchGroups[2];
    this->m_displacementPosition.y = std::stof(subMatch.str());

    // Displacement Position
    subMatch = matchGroups[3];
    this->m_startPosition = sf::Vector2f(std::stof(subMatch.str()), 0);
    subMatch = matchGroups[4];
    this->m_startPosition.y = std::stof(subMatch.str());
  
    // 5 = Colour, 6 = TextureID (Front side)
	if (matchGroups[5] != "") {
		// Default colour texture
		m_frontMaterial.textureId = 0;

		subMatch = matchGroups[5];
		m_frontMaterial.colour = stringToColor(subMatch.str());
	} else {
        subMatch = matchGroups[6];
		m_frontMaterial.textureId = std::stoi(subMatch.str());

		m_frontMaterial.colour = { 1.0f, 1.0f, 1.0f };
	}


    // 7 = Colour, 8 = TextureID (Back side)
	if (matchGroups[7] != "") {
		// Default colour texture
		m_backMaterial.textureId = 0;

		subMatch = matchGroups[7];
		m_backMaterial.colour = stringToColor(subMatch.str());
	}
	else {
		subMatch = matchGroups[8];
		m_backMaterial.textureId = std::stoi(subMatch.str());;

		m_backMaterial.colour = { 1.0f, 1.0f, 1.0f };
	}

    // If 10 doesn't exist then 9 = Level, else 9 = Z_Index
	int z_index, level;
    if (matchGroups[10] != "") {
        subMatch = matchGroups[9];    // Z_Index
		z_index = std::stoi(subMatch.str());

        subMatch = matchGroups[10];   // Level
		level = std::stoi(subMatch.str());

    } else {
        subMatch = matchGroups[9];    // Level
		level = std::stoi(subMatch.str());

		z_index = 1;
    }

	m_level = level;

	// Find mix and max heights from Z-Index (0 -> 1)
	switch (z_index) {
		case 2:  m_endHeight = 3 / 4.f;  m_startHeight = 0.f; break;
		case 3:  m_endHeight = 2 / 4.f;  m_startHeight = 0.f; break;
		case 4:  m_endHeight = 1 / 4.f;  m_startHeight = 0.f; break;

		case 5:  m_endHeight =  2 / 4.f;  m_startHeight =  1 / 4.f; break;
		case 6:  m_endHeight =  3 / 4.f;  m_startHeight =  2 / 4.f; break;
		case 7:  m_endHeight =  4 / 4.f;  m_startHeight =  3 / 4.f; break;

		case 8:  m_endHeight =  4 / 4.f;  m_startHeight =  2 / 4.f; break;
		case 9:  m_endHeight =  4 / 4.f;  m_startHeight =  1 / 4.f; break;
		case 10: m_endHeight =  3 / 4.f;  m_startHeight =  1 / 4.f; break;

		default: m_endHeight =  4 / 4.f;  m_startHeight = 0.f;	  break;
	}

    this->type = "WALL";
}

CYWall::CYWall(const json & jObj)
{
	this->m_startPosition			= { jObj["START_POS"][0], jObj["START_POS"][1] };
	this->m_displacementPosition	= { jObj["DISP_POS"][0], jObj["DISP_POS"][1] };

	this->m_frontMaterial.textureId = jObj["MAT1"]["tex"];
	this->m_backMaterial.textureId	= jObj["MAT2"]["tex"];
	this->m_frontMaterial.deserializeColour(jObj["MAT1"]["col"]);
	this->m_backMaterial.deserializeColour(jObj["MAT2"]["col"]);

	this->m_level					= jObj["LEVEL"];
	this->m_startHeight				= jObj["HEIGHT"][0];
	this->m_endHeight				= jObj["HEIGHT"][1];

	this->type = "WALL";
}


void CYWall::createModel() {

    Mesh mesh;
	glm::vec3 vertex[4];

	// Get geometric properties of the wall
	float minHeight = (m_level + m_startHeight) / GLOBAL_WORLD_HEIGHT;
	float maxHeight = (m_level + m_endHeight)   / GLOBAL_WORLD_HEIGHT;
	glm::vec2 wallOrigin = { m_startPosition.x, m_startPosition.y };
	glm::vec2 wallFinish = { m_startPosition.x + m_displacementPosition.x, m_startPosition.y + m_displacementPosition.y };
    
	// Set the vertices of the wall using such properties
	vertex[1] = glm::vec3((wallOrigin.x) / GLOBAL_WORLD_SIZE, maxHeight, (wallOrigin.y) / GLOBAL_WORLD_SIZE);
	vertex[2] = glm::vec3((wallFinish.x) / GLOBAL_WORLD_SIZE, maxHeight, (wallFinish.y) / GLOBAL_WORLD_SIZE);
	vertex[3] = glm::vec3((wallFinish.x) / GLOBAL_WORLD_SIZE, minHeight, (wallFinish.y) / GLOBAL_WORLD_SIZE);
	vertex[0] = glm::vec3((wallOrigin.x) / GLOBAL_WORLD_SIZE, minHeight, (wallOrigin.y) / GLOBAL_WORLD_SIZE);

	// Use the cross product to determine the normal
	glm::vec3 normal = glm::cross(vertex[2] - vertex[1], vertex[3] - vertex[1]);

	// Reduce the need for duplicated vertices by using indices
    mesh.indices = {0, 3, 1, 1, 2, 3};

	// Add the calculated 3d properties into the mesh for constructing
	// Vertices
	mesh.vertices.insert(mesh.vertices.end(), { vertex[1].x, vertex[1].y, vertex[1].z });
	mesh.vertices.insert(mesh.vertices.end(), { vertex[2].x, vertex[2].y, vertex[2].z });
	mesh.vertices.insert(mesh.vertices.end(), { vertex[3].x, vertex[3].y, vertex[3].z });
	mesh.vertices.insert(mesh.vertices.end(), { vertex[0].x, vertex[0].y, vertex[0].z });

	// Normals
	for (int i = 0; i < 4; i++)
		mesh.normals.insert(mesh.normals.end(), { normal.x, normal.y, normal.z });

	// Colors
	for (int i = 0; i < 4; i++)
		mesh.colour.insert(mesh.colour.end(), { m_frontMaterial.colour[0],
			m_frontMaterial.colour[1],
            m_frontMaterial.colour[2] });

	// default
    mesh.texCoords =
    {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };



    m_geometry.create(mesh);
}

void CYWall::render(Renderer& renderer)
{
    renderer.draw(m_geometry);
}

void CYWall::toJsonFormat(json& jLevel, int id)
{
	std::string itemId = std::to_string(id);

	// Add in the properties to JSON
	jLevel["START_POS"]		= { m_startPosition.x, m_startPosition.y };

	jLevel["DISP_POS"]		= { m_displacementPosition.x, m_displacementPosition.y };

	jLevel["MAT1"]["tex"]	= this->m_frontMaterial.textureId;
	jLevel["MAT1"]["col"]	= this->m_frontMaterial.serializeColour();
	jLevel["MAT2"]["tex"]	= this->m_backMaterial.textureId;
	jLevel["MAT2"]["col"]	= this->m_backMaterial.serializeColour();

    jLevel["LEVEL"]			= this->m_level;
	jLevel["HEIGHT"]		= { this->m_startHeight, this->m_endHeight };

    return;
}
