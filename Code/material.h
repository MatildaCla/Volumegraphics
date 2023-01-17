#ifndef MATERIAL_H
#define MATERIAL_H

#include "framework.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "extra/hdre.h"

class Material {
public:

	Shader* shader = NULL;
	Texture* texture = NULL;
	vec4 color;

	virtual void setUniforms(Camera* camera, Matrix44 model) = 0;
	virtual void render(Mesh* mesh, Matrix44 model, Camera * camera) = 0;
	virtual void renderInMenu() = 0;
};

class StandardMaterial : public Material {
public:

	StandardMaterial();
	~StandardMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera * camera);
	void renderInMenu();
};

class WireframeMaterial : public StandardMaterial {
public:

	WireframeMaterial();
	~WireframeMaterial();

	void render(Mesh* mesh, Matrix44 model, Camera * camera);
};

// TODO: Derived class VolumeMaterial
class VolumeMaterial : public StandardMaterial {
public:

	float ray_step;
	float brightness;
	int classification_picker;
	Texture* jittering_table;
	Texture* u_transfer_function;
	bool jittering = true;
	bool jitterFunction = false;
	bool clipping = false;
	Vector4 clipping_plane;
	float k;

	float transferMax_red = 1.0f;
	float transferMin_red = 0.28f;
	float transferMax_green = 0.3f;
	float transferMin_green = 0.27f;
	float transferMax_blue = 0.11f;
	float transferMin_blue = 0.04f;

	Texture* transfer_bone = NULL;
	Texture* transfer_organs = NULL;
	Texture* transfer_muscle = NULL;
	const char* items[4] = { "Bone", "Organs", "None", "Custom" };
	const char* current_item = "Transfer Function";
	float current_transfer = 0;

	VolumeMaterial();
	~VolumeMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera* camera);
	void renderInMenu();

};



#endif