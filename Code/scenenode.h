#ifndef SCENENODE_H
#define SCENENODE_H

#include "framework.h"

#include "shader.h"
#include "mesh.h"
#include "camera.h"
#include "material.h"

class Light;

class SceneNode {
public:

	static unsigned int lastNameId;

	SceneNode();
	SceneNode(const char* name);
	~SceneNode();

	Material * material = NULL;
	std::string name;

	Mesh* mesh = NULL;
	Matrix44 model;

	virtual void render(Camera* camera);
	virtual void renderWireframe(Camera* camera);
	virtual void renderInMenu();
};

// TODO: Derived class VolumeNode

class VolumeNode : public SceneNode {
public:

	Texture* texture;
	std::vector<std::string> texture_names;
	std::vector<Texture*> textures;
	int texture_id;
	int material_id;

	VolumeNode();
	VolumeNode(const char* name);
	~VolumeNode();

	void declareTextures(int num_textures);
	void setTexture(int index);
	void setMaterial();
	void renderInMenu();
};

#endif