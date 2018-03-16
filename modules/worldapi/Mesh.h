#pragma once

#include "worldapidef.h"

#include <string>
#include <vector>
#include <stdexcept>

#include "maths/MathsHelper.h"

class Vertex {
public :
	Vertex() : _position{}, _normal{}, _texture{} {}

	maths::vec3d getPosition() const { return _position; }
	maths::vec3d getNormal() const { return _normal; }
	maths::vec2d getTexture() const { return _texture; }

	void setPosition(const maths::vec3d & position) {
		_position = position;
	}

	void setPosition(double x, double y, double z) {
		_position = { x, y, z };
	}

	void setNormal(const maths::vec3d & normal) {
		_normal = normal;
	}

	void setNormal(double x, double y, double z) {
		_normal = { x, y, z };
	}

	void setTexture(const maths::vec2d &texture) {
		_texture = texture;
	}

	void setTexture(double x, double y) {
		_texture = { x, y };
	}

private:
	maths::vec3d _position;
	maths::vec3d _normal;
	maths::vec2d _texture;
};


class WORLDAPI_EXPORT Face {

public :
	Face();
	virtual ~Face();

	void addID(int id);
	void setID(int vert, int id);
	int getID(int vert) const;
	const std::vector<int> & getIDs() const;

	int vertexCount() const;
private :
	std::vector<int> _ids;
};


class WORLDAPI_EXPORT Mesh {

public :
	Mesh();
	virtual ~Mesh();

	/* Ajoute une face au mesh. Cette m�thode n'effectue aucune
	v�rification quand � l'existence des vertices correspondant
	aux indices de la face. */
	void addFace(Face & face);
	const std::vector<Face> & getFaces() const;

	void addVertex(const Vertex & vertex);
	Vertex & getVertex(int id);
	const Vertex & getVertex(int id) const;
	int getCount() const;
	const std::vector<Vertex> & getVertices() const;

	void clearVertices();
	void allocateVertices(int capacity);
	void allocateFaces(int capacity);

	void setMaterialName(const std::string & material);
	const std::string & getMaterialName() const;
private:
	std::string _materialName;

	std::vector<Vertex> _vertices;
	std::vector<Face> _faces;
};
