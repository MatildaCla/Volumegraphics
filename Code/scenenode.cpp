#include "scenenode.h"
#include "application.h"
#include "texture.h"
#include "utils.h"
#include "volume.h"

unsigned int SceneNode::lastNameId = 0;
unsigned int mesh_selected = 0;

SceneNode::SceneNode()
{
	this->name = std::string("Node" + std::to_string(lastNameId++));
}

SceneNode::SceneNode(const char * name)
{
	this->name = name;
}

SceneNode::~SceneNode()
{

}

void SceneNode::render(Camera* camera)
{
	if (material)
		material->render(mesh, model, camera);
}

void SceneNode::renderWireframe(Camera* camera)
{
	WireframeMaterial mat = WireframeMaterial();
	mat.render(mesh, model, camera);
}

void SceneNode::renderInMenu()
{
	//Model edit
	if (ImGui::TreeNode("Model")) 
	{
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(model.m, matrixTranslation, matrixRotation, matrixScale);
		ImGui::DragFloat3("Position", matrixTranslation, 0.1f);
		ImGui::DragFloat3("Rotation", matrixRotation, 0.1f);
		ImGui::DragFloat3("Scale", matrixScale, 0.1f);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, model.m);
		
		ImGui::TreePop();
	}

	//Material
	if (material && ImGui::TreeNode("Material"))
	{
		material->renderInMenu();
		ImGui::TreePop();
	}

	//Geometry
	if (mesh && ImGui::TreeNode("Geometry"))
	{
		bool changed = false;
		changed |= ImGui::Combo("Mesh", (int*)&mesh_selected, "SPHERE\0");

		ImGui::TreePop();
	}
}


VolumeNode::VolumeNode()
{
	this->name = std::string("Volume" + std::to_string(lastNameId++));
	this->texture_id = 0;
	this->material_id = 0;
	this->texture_names = { "data/volumes/teapot_16_16.png", "data/volumes/bonsai_16_16.png", "data/volumes/foot_16_16.png", "data(volumes/CT-Abdomen.pvm", "data/volumes/Orange.pvm"};
	setMaterial();
}



VolumeNode::VolumeNode(const char* name)
{
	this->name = name;
	this->texture_id = 0;
	this->material_id = 0;
	this->texture_names = { "data/volumes/teapot_16_16.png", "data/volumes/bonsai_16_16.png", "data/volumes/foot_16_16.png", "data/volumes/CT-Abdomen.pvm", "data/volumes/Orange.pvm" };
	setMaterial();
}

VolumeNode::~VolumeNode()
{

}

void VolumeNode::renderInMenu()
{
	//Model edit
	if (ImGui::TreeNode("Model"))
	{
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(model.m, matrixTranslation, matrixRotation, matrixScale);
		ImGui::DragFloat3("Position", matrixTranslation, 0.1f);
		ImGui::DragFloat3("Rotation", matrixRotation, 0.1f);
		ImGui::DragFloat3("Scale", matrixScale, 0.1f);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, model.m);

		ImGui::TreePop();
	}

	//Material
	if (material && ImGui::TreeNode("Material"))
	{
		bool changed = false;
		changed |= ImGui::Combo("Material", &material_id, "STANDARD RAY MARCH.\0");
		if (changed) {
			setMaterial();
		}
		material->renderInMenu();
		ImGui::TreePop();
	}

	//Volume data
	if (texture && ImGui::TreeNode("Volume")) {
		bool changed = false;
		if (textures.size() == 1) {
			changed |= ImGui::Combo("Volume", &texture_id, "TEAPOT\0");
		}
		else if (textures.size() == 2) {
			changed |= ImGui::Combo("Volume", &texture_id, "TEAPOT\0BONSAI\0");
		}
		else if (textures.size() == 3) {
			changed |= ImGui::Combo("Volume", &texture_id, "TEAPOT\0BONSAI\0FOOT\0");
		}
		else if (textures.size() == 4) {
			changed |= ImGui::Combo("Volume", &texture_id, "TEAPOT\0BONSAI\0FOOT\0ABDOMEN\0");
		}

		else if (textures.size() == 5) {
			changed |= ImGui::Combo("Volume", &texture_id, "TEAPOT\0BONSAI\0FOOT\0ABDOMEN\0ORANGE\0");
		}

		if (changed) {
			std::cout << texture_id << std::endl;
			setTexture(texture_id);
		}
		ImGui::TreePop();
	}
}



void VolumeNode::declareTextures(int num_textures) {

	int n = (int)clamp((float)num_textures, 0.0, 5.0);
	for (int i = 0; i < n; i++) {
		std::cout << this->texture_names[i] << std::endl;
		Volume* vol = new Volume();
		if (i == 3 || i == 4) {
			vol->loadPVM(this->texture_names[i].c_str());
		}

		
		else {
			vol->loadPNG(this->texture_names[i].c_str(), 16, 16);
		}
		
		Texture* tex = new Texture();
		tex->create3DFromVolume(vol, GL_CLAMP_TO_EDGE);
		textures.push_back(tex);
	}

	if (textures.size() > 0) {
		setTexture(0);
	}


}

void VolumeNode::setTexture(int index) {
	this->texture = textures[index];
	this->material->texture = texture;
}

void VolumeNode::setMaterial() {
		this->material = new VolumeMaterial();
}



