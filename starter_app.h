#ifndef _STARTER_APP_H
#define _STARTER_APP_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include <maths/matrix44.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include "primitive_builder.h"
#include <graphics/scene.h>
#include "CollisionDetector.h"
#include "GameObject.h"
#include "PicrossLevel.h"
#include "input/touch_input_manager.h"
#include <input/keyboard.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class InputManager;
}

class StarterApp : public gef::Application
{
public:
	StarterApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
	void SetupCamera();

	gef::SpriteRenderer* sprite_renderer_;
	gef::Renderer3D* renderer_3d_;
	gef::InputManager* input_manager_;
	gef::Font* font_;

	float fps_;

	gef::Vector4 camera_eye_;
	gef::Vector4 camera_lookat_;
	gef::Vector4 camera_up_;
	float camera_fov_;
	float near_plane_;
	float far_plane_;

	gef::MeshInstance* player_;

	PrimitiveBuilder* primitive_builder_;

	//New Stuff
	void loadScenes();
	gef::Mesh* GetFirstMesh(gef::Scene* scene);

	//Balls
	gef::Scene* scene1;
	gef::Mesh* scene1Mesh;

	gef::Scene* scene2;
	gef::Mesh* scene2Mesh;
	GameObject* ball2Mesh;
	GameObject wall1;
	GameObject testCube;
	gef::Mesh* testCubeMesh;
	PicrossLevel* pLevel;
	float picrossSpacing;

	//Collision Detection
	CollisionDetector collisionDetector;
	bool collision;
	bool keyW;
	
	//Input
	gef::Vector2 touchPosition;
	gef::Vector4 rayDirValues;
	std::pair<int, bool> pushingControls[3];
	gef::Keyboard::KeyCode destroyKey;
	gef::Keyboard::KeyCode protectKey;
	bool destroyButtonDown;
	bool protectButtonDown;

	//Camera
	float cameraDist;
	float cameraYOffset;
	float cameraXZOffset;
	float cameraRotAmount;

	//Numbers
	static constexpr int numNumbers = 4;
	std::pair<gef::Scene*, gef::MeshInstance*> numberScenes[numNumbers];
};

#endif // _STARTER_APP_H
