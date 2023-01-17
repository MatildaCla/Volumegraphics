#include "material.h"
#include "texture.h"
#include "application.h"
#include "extra/hdre.h"

StandardMaterial::StandardMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

StandardMaterial::~StandardMaterial()
{

}

void StandardMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);
	shader->setUniform("u_time", Application::instance->time);
	shader->setUniform("u_color", color);

	if (texture)
		shader->setUniform("u_texture", texture);
}

void StandardMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void StandardMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
}

WireframeMaterial::WireframeMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

WireframeMaterial::~WireframeMaterial()
{

}

void WireframeMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (shader && mesh)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//enable shader
		shader->enable();

		//upload material specific uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

VolumeMaterial::VolumeMaterial() {
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/ray_marching.fs");
	ray_step = 0.01f;
	brightness = 5.f;
	classification_picker = 0;
	jittering_table = Texture::Get("data/images/blueNoise.png");
	u_transfer_function = Texture::Get("data/tfabd/organe.png");
	transfer_bone = Texture::Get("data/images/knochen.PNG");
	transfer_organs = Texture::Get("data/images/organe.png");
	k = 1.f;
	clipping_plane = Vector4(0.f, 0.f, 1.f, k);

	current_transfer = 0;

	
}

VolumeMaterial::~VolumeMaterial() {

}

void VolumeMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera) 
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//enable alpha blending 
		glEnable(GL_BLEND);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable alpha blending 
		glDisable(GL_BLEND);

		//disable shader
		shader->disable();
	}
}

void VolumeMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	StandardMaterial::setUniforms(camera, model);

	Matrix44 inv_model = model;
	inv_model.inverse();
	shader->setUniform("u_world_2_local", inv_model); // explicar al report! 
	shader->setUniform("u_ray_step", ray_step);
	shader->setUniform("u_brightness", brightness);

	shader->setUniform("u_classification_picker", classification_picker);
	shader->setUniform("u_texture_width", jittering_table->width);
	shader->setUniform("u_jittering_table", jittering_table);
	shader->setUniform("u_jittering", jittering);
	shader->setUniform("u_jitter_function", jitterFunction);
	shader->setUniform("u_transfer_function", u_transfer_function);
	shader->setUniform("u_clipping_plane", clipping_plane);

	// send transfer function texutres to shader
	shader->setUniform("u_current_transfer", current_transfer);

	// Parameters for custom transfer function
	shader->setUniform("u_transfer_max_red", transferMax_red);
	shader->setUniform("u_transfer_min_red", transferMin_red);
	shader->setUniform("u_transfer_max_blue", transferMax_blue);
	shader->setUniform("u_transfer_min_blue", transferMin_blue);
	shader->setUniform("u_transfer_max_green", transferMax_green);
	shader->setUniform("u_transfer_min_green", transferMin_green);

}

void VolumeMaterial::renderInMenu()
{
	StandardMaterial::renderInMenu();
	ImGui::SliderFloat("Step Length", (float*)&ray_step, 0.001f, 1.f);
	ImGui::SliderFloat("Brightness", (float*)&brightness, 1.f, 20.f);
	ImGui::Combo("Classification", &classification_picker, "CONSTANT\0GREEN\0TF\0");
	ImGui::Checkbox("Jittering Noise", &jittering);
	ImGui::Checkbox("Jittering Function", &jitterFunction);
	ImGui::Checkbox("Clipping Plane",&clipping); 
	if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(items[n], is_selected))
				current_item = items[n];
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}

	current_transfer = 1;
	if (current_item == "Bone")
	{
		u_transfer_function = transfer_bone;
	}
	else if (current_item == "Organs")
	{
		u_transfer_function = transfer_organs;
	}

	else if (current_item == "Custom")
	{
		current_transfer = 3;
		ImGui::SliderFloat("Red Max", (float*)&transferMax_red, 0.0, 1.0, "%.2f");
		ImGui::SliderFloat("Red Min", (float*)&transferMin_red, 0.0, 1.0, "%.2f");
		ImGui::SliderFloat("Green Max", (float*)&transferMax_green, 0.0, 1.0, "%.2f");
		ImGui::SliderFloat("Green Min", (float*)&transferMin_green, 0.0, 1.0, "%.2f");
		ImGui::SliderFloat("Blue Max", (float*)&transferMax_blue, 0.0, 1.0, "%.2f");
		ImGui::SliderFloat("Blue Min", (float*)&transferMin_blue, 0.0, 1.0, "%.2f");
	}
	else
	{
		current_transfer = 0;
	}
	if (clipping == true) {
		bool changed = true;
		changed |= ImGui::DragFloat("Clipping plane z", (float*)&k, 0.05f, -4.f, 4.f);
		if (changed) {
			clipping_plane = Vector4(0.f, 0.f, 1.f, k);
		}
	}	
}

