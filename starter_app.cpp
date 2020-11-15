#include "starter_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <maths/vector2.h>
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <input/keyboard.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/ImGui/imgui.h>
#include <graphics/ImGui/imgui_impl_dx11.h>
#include <graphics/ImGui/imgui_impl_win32.h>

StarterApp::StarterApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	font_(NULL),
	renderer_3d_(NULL)
{
}

void StarterApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	renderer_3d_ = gef::Renderer3D::Create(platform_);
	input_manager_ = gef::InputManager::Create(platform_);

	InitFont();
	SetupCamera();
	SetupLights();

	loadScenes();

	gef::Sphere new_bounds(gef::Vector4(0.0f, 0.0f, 0.0f), 50.5f);
	scene1Mesh->set_bounding_sphere(new_bounds);
	scene2Mesh->set_bounding_sphere(new_bounds);

	primitive_builder_ = new PrimitiveBuilder(platform_);
	//player_.set_mesh(primitive_builder_->GetDefaultCubeMesh());
	player_ = new gef::MeshInstance();
	player_->set_mesh(scene1Mesh);
	ball2Mesh = new GameObject;
	ball2Mesh->set_mesh(scene2Mesh);
	gef::Matrix44 ball2Transform = ball2Mesh->transform();
	ball2Transform.SetTranslation(gef::Vector4(100.5f, 100.5f, 0.0f));
	ball2Mesh->set_transform(ball2Transform);
	ball2Mesh->setPosition(gef::Vector4(100.5f, 100.5f, 0.0f));
	//ball2Mesh->transform().

	testCubeMesh = primitive_builder_->CreateBoxMesh(gef::Vector4(25.0f, 25.0f, 25.0f));
	testCube.set_mesh(testCubeMesh);

	wall1.set_mesh(primitive_builder_->CreateBoxMesh(gef::Vector4(25.0f, 50.0f, 12.5f)));


	collision = false;
	
	pLevel = new PicrossLevel(primitive_builder_);
	picrossSpacing = 0.0f;
}

void StarterApp::CleanUp()
{
	delete primitive_builder_;
	primitive_builder_ = NULL;

	CleanUpFont();

	delete input_manager_;
	input_manager_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete scene1;
	scene1 = NULL;

	delete scene1Mesh;
	scene1Mesh = NULL;

	delete pLevel;
	pLevel = NULL;
}

bool StarterApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;


	// read input devices
	if (input_manager_)
	{
		input_manager_->Update();

		// controller input
		gef::SonyControllerInputManager* controller_manager = input_manager_->controller_input();
		if (controller_manager)
		{
		}

		// keyboard input
		gef::Keyboard* keyboard = input_manager_->keyboard();
		if (keyboard)
		{
		}
	}

	//collision = collisionDetector.sphereToSphere(player_, ball2Mesh);
	collision = collisionDetector.AABB(player_, ball2Mesh);
	ball2Mesh->setRotation(gef::Vector4(ball2Mesh->getRotation().x() + frame_time, ball2Mesh->getRotation().y() + frame_time, ball2Mesh->getRotation().z() + frame_time));
	//ball2Mesh->setPosition(gef::Vector4(ball2Mesh->getPostition().x(), ball2Mesh->getPostition().y() + frame_time, ball2Mesh->getPostition().z()));
	pLevel->setSpacing(picrossSpacing);

	return true;
}

void StarterApp::Render()
{
	gef::Matrix44 projection_matrix;
	gef::Matrix44 view_matrix;

	projection_matrix = platform_.PerspectiveProjectionFov(camera_fov_, (float)platform_.width() / (float)platform_.height(), near_plane_, far_plane_);
	view_matrix.LookAt(camera_eye_, camera_lookat_, camera_up_);
	renderer_3d_->set_projection_matrix(projection_matrix);
	renderer_3d_->set_view_matrix(view_matrix);

	// draw meshes here
	renderer_3d_->Begin();


	// draw 3d stuff here
	//renderer_3d_->DrawMesh(*player_);

	//renderer_3d_->DrawMesh(*ball2Mesh);

	//renderer_3d_->DrawMesh(static_cast<gef::MeshInstance>(wall1));
	pLevel->render(renderer_3d_);

	//renderer_3d_->DrawMesh(testCube);

	renderer_3d_->End();

	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}
void StarterApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void StarterApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void StarterApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);

		std::string collisionText = "-";
		if (collision)
		{
			collisionText = "Collision";
		}
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 460.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, collisionText.c_str());
	}
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//Create ImGui Windows
	ImGui::Begin("Test");

	ImGui::DragFloat("Spacing", &picrossSpacing, 0.5f, 0.0f, 10.0f, "%.2f");

	ImGui::End();


	//Assemble Together Draw Data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void StarterApp::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, -500.0f, 100.0f));

	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void StarterApp::SetupCamera()
{
	// initialise the camera settings
	camera_eye_ = gef::Vector4(5.0f, 5.0f, -350.0f);
	camera_lookat_ = gef::Vector4(0.0f, 0.0f, 0.0f);
	camera_up_ = gef::Vector4(0.0f, 1.0f, 0.0f);
	camera_fov_ = gef::DegToRad(45.0f);
	near_plane_ = 0.01f;
	far_plane_ = 100.f;
}

void StarterApp::loadScenes()
{
	//Scene1
	scene1 = new gef::Scene();
	scene1->ReadSceneFromFile(platform_, "ball1.scn");

	scene1->CreateMaterials(platform_);

	scene1Mesh = GetFirstMesh(scene1);

	//Scene2
	scene2 = new gef::Scene();
	scene2->ReadSceneFromFile(platform_, "ball2.scn");

	scene2->CreateMaterials(platform_);

	scene2Mesh = GetFirstMesh(scene2);
}

gef::Mesh * StarterApp::GetFirstMesh(gef::Scene * scene)
{
	gef::Mesh* mesh = NULL;

	if (scene)
	{
		// now check to see if there is any mesh data in the file, if so lets create a mesh from it
		if (scene->mesh_data.size() > 0)
		{
			mesh = scene->CreateMesh(platform_, scene->mesh_data.front());
			
		}
	}

	return mesh;
}
