#include "lab_m1/Tema2/tema2.h"

#include "lab_m1/Tema2/utils/functions.h"
#include "lab_m1/Tema2/utils/constants.h"
#include "lab_m1/Tema2/utils/my_camera.h"
#include "lab_m1/Tema2/elements/drone.h"
#include "lab_m1/Tema2/elements/delivery.h"

#include "lab_m1/Tema2/elements/simple_objects.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


DroneScene::DroneScene()
{
}


DroneScene::~DroneScene()
{
}

void DroneScene::InitDroneView() {
	drone_camera = new MyCamera();
	projectionMatrix = glm::perspective(DRONE_CAMERA_ANGLE, window->props.aspectRatio, 0.05f, DRONE_CAMERA_DISTANCE);
}

void DroneScene::InitMapView() {
	map_camera = new MyCamera();
	mapProjectionMatrix = glm::ortho(-MAP_CAMERA_SIZE, MAP_CAMERA_SIZE, -MAP_CAMERA_SIZE, MAP_CAMERA_SIZE, 0.05f, 2 * MAP_CAMERA_DISTANCE);
}

void DroneScene::UpdateMapView() {

	glm::vec3 map_camera_pos = glm::vec3(0, MAP_CAMERA_DISTANCE, 0);
	glm::vec3 map_camera_forward = glm::vec3(0, -1, 0);

	glm::vec3 map_camera_up = player_drone->GetForwardDir();
	map_camera_up.y = 0;

	map_camera->SetVectors(map_camera_pos, map_camera_forward, map_camera_up);
}

void DroneScene::Init()
{
    // Initializare shadrer normal
	defaultShader = new Shader("DefaultShader");
	defaultShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
	defaultShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
	defaultShader->CreateAndLink();
	shaders["DefaultShader"] = defaultShader;

	// Initializare shadrer pentru teren
	terrainShader = new Shader("TerrainShader");
	terrainShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "TerrainVertexShader.glsl"), GL_VERTEX_SHADER);
	terrainShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "TerrainFragShader.glsl"), GL_FRAGMENT_SHADER);
	terrainShader->CreateAndLink();
	shaders["TerrainShader"] = terrainShader;

	// Viziune drona
	InitDroneView();

	// Viziune harta
	InitMapView();

	terrain = new DroneTerrain();

	player_drone = new Drone();
	player_drone->Init(true);

	handler = new RandomPositionHandler(NUM_BUILDINGS + NUM_TREES + 50); // inca 20 pozitii random pt cutii

	delivery_manager = new DeliveryManager();
	delivery_manager->Init(player_drone, handler);

	view_mode = ViewMode::FirstPerson;
	SetViewMode(ThirdPerson);
	InitDroneView();
    
	trees.resize(NUM_TREES);
	for (int i = 0; i < NUM_TREES; i++) {
		trees[i] = new Tree();
		trees[i]->Init(handler);	
	}

	buildings.resize(NUM_BUILDINGS);
	for (int i = 0; i < NUM_BUILDINGS; i++) {
		buildings[i] = new Building();
		buildings[i]->Init(handler);
	}

	//// PT OBIECTE TRANSPARENTE
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
}

void DroneScene::UpdateScreen() {
	glm::ivec2 resolution = window->GetResolution(); // Window resolution

	// Randare drona
	glViewport(0, 0, resolution.x, resolution.y);
	RenderAllMeshes(drone_camera, projectionMatrix);

	// Randare mini-harta
	UpdateMapView();

	int mapSize = min(resolution.x, resolution.y) * 0.6f;
	int padding = 10;

	int mapX = resolution.x - mapSize * 0.8f - padding;
	int mapY = resolution.y - mapSize * 0.8f - padding;

	glViewport(mapX, mapY, mapSize, mapSize);

	DeliveryBox* box = delivery_manager->GetBox();
	// Crestem temporal dimensiunile de randare ale cutiei si dronei pe harta
	player_drone->SetScale(ALL, TEMP_MAP_DRONE_SCALE);
	if (box->IsAttached()) {
		// Daca cutia este atasata, nu o randam pentru a nu "acoperi" imaginea dronei
		player_drone->SetScale(BOX_INDEX, glm::vec3(0));
		player_drone->SetScale(ROPE_INDEX, glm::vec3(0));
	}
	player_drone->PrepareData();

	box->SetScale(ALL, TEMP_MAP_BOX_SCALE);
	box->PrepareData();

	glDisable(GL_DEPTH_TEST);
	RenderAllMeshes(map_camera, mapProjectionMatrix);
	glEnable(GL_DEPTH_TEST);

	// Resetam dimensiunea de randare a cutiei si dronei pe harta
	player_drone->SetScale(ALL, glm::vec3(1.0f));
	player_drone->PrepareData();
	box->IsAttached() ? box->SetScale(ALL, glm::vec3(0)) : box->SetScale(ALL, glm::vec3(1.0f));
	box->PrepareData();
}

void DroneScene::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void DroneScene::Update(float deltaTimeSeconds)
{
	player_drone->Update(deltaTimeSeconds, trees, buildings);
	delivery_manager->Update(deltaTimeSeconds);

	UpdateDroneView();
	UpdateScreen();
}


void DroneScene::FrameEnd()
{

}

void DroneScene::OnInputUpdate(float deltaTime, int mods)
{
	if (window->KeyHold(GLFW_KEY_W)) {
		player_drone->IncreaseThrust(deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_S)) {
		player_drone->DecreaseThrust(deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_DOWN)) {
		player_drone->IncreasePitch(deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_UP)) {
		player_drone->DecreasePitch(deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_A)) {
		player_drone->IncreaseYaw(deltaTime);
	}

    if (window->KeyHold(GLFW_KEY_D)) {
		player_drone->DecreaseYaw(deltaTime);
    }

	if (window->KeyHold(GLFW_KEY_RIGHT)) {
		player_drone->IncreaseRoll(deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_LEFT)) {
		player_drone->DecreaseRoll(deltaTime);
	}


	// PT TESTING
	if (window->KeyHold(GLFW_KEY_I)) {
		player_drone->TestTranslateForward(deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_K)) {
		player_drone->TestTranslateBackward(deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_J)) {
		player_drone->TestRotateLeft(deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_L)) {
		player_drone->TestRotateRight(deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_U)) {
		player_drone->TestTranslateDown(deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_O)) {
		player_drone->TestTranslateUp(deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_0)) {
		player_drone->MakeDroneStraight();
	}
}


void DroneScene::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE) {
		delivery_manager->ReleaseBox();
	}

	if (key == GLFW_KEY_F) {
		SetViewMode(ViewMode::FirstPerson);
	}

	if (key == GLFW_KEY_T) {
		SetViewMode(ViewMode::ThirdPerson);
	}

	if (key == GLFW_KEY_G) {
		SetViewMode(ViewMode::BottomUp);
	}
}


void DroneScene::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void DroneScene::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.0025f;
        float sensivityOY = 0.0025f;

		if (window->GetSpecialKeyState() == 0) {
			if (view_mode == ViewMode::ThirdPerson) {
				drone_camera->RotateThirdPerson_OY(-sensivityOX * deltaX);
				drone_camera->RotateThirdPerson_OX(-sensivityOY * deltaY);
			}

			if (view_mode == ViewMode::BottomUp) {
				drone_camera->RotateFirstPerson_OY(-sensivityOX * deltaX);
				drone_camera->RotateFirstPerson_OX(-sensivityOY * deltaY);
			}
        }
	}
}


void DroneScene::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void DroneScene::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void DroneScene::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void DroneScene::OnWindowResize(int width, int height)
{
}

void DroneScene::RenderAllMeshes(MyCamera* camera, glm::mat4& projMatrix) {
	terrain->RenderTerrain(terrainShader, camera, projMatrix);

	delivery_manager->PrepareAndRender(defaultShader, camera, projMatrix);

	// player_drone->PrepareData(); (randam deja dupa miscare sa fie mai smooth)
	player_drone->Render(defaultShader, camera, projMatrix);

	for (int i = 0; i < NUM_TREES; i++) {
		trees[i]->PrepareData();
		trees[i]->Render(defaultShader, camera, projMatrix);
	}
	for (int i = 0; i < NUM_BUILDINGS; i++) {
		buildings[i]->PrepareData();
		buildings[i]->Render(defaultShader, camera, projMatrix);
	}
}

void DroneScene::UpdateDroneView() {
	glm::vec3 drone_pos = player_drone->GetWorldPos();
	glm::vec3 forward_dir = player_drone->GetForwardDir();
	glm::vec3 up_dir = player_drone->GetUpDir();
	glm::vec3 offset;

	if (view_mode == ViewMode::FirstPerson) {
		offset = -forward_dir * FIRST_PERSON_DISTANCE_RATIO + glm::vec3(0, 0.25f, 0);
		drone_camera->SetVectors(drone_pos + offset, forward_dir, up_dir);
	}

	if (view_mode == ViewMode::ThirdPerson) {
		drone_camera->SetTarget(drone_pos);
		drone_camera->UpdateCameraPositionThirdPerson();
	}

	if (view_mode == ViewMode::BottomUp) {
		offset = glm::vec3(0.0f, -BOTTOM_UP_DISTANCE, 0.0f);
		drone_camera->SetPos(drone_pos + offset);
	}
}

void DroneScene::SetViewMode(ViewMode mode) {
	// Verificam daca modul de vizualizare este deja setat
	if (view_mode == mode) {
		return;
	}

	view_mode = mode;

	glm::vec3 drone_pos = player_drone->GetWorldPos();
	glm::vec3 forward_dir = player_drone->GetForwardDir();
	glm::vec3 up_dir = player_drone->GetUpDir();
	glm::vec3 offset;

	if (mode == ViewMode::FirstPerson) {
		offset = -forward_dir * FIRST_PERSON_DISTANCE_RATIO + glm::vec3(0, 0.25f, 0);
		drone_camera->SetVectors(drone_pos + offset, forward_dir, up_dir);
		player_drone->SetScale(DRONE_ARROW_INDEX, glm::vec3(0));
	}

	if (mode == ViewMode::ThirdPerson) {
		drone_camera->InitThirdPerson(drone_pos, THIRD_PERSON_DISTANCE, 0, 20.0f);
		player_drone->SetScale(DRONE_ARROW_INDEX, glm::vec3(1.0f));
	}

	if (mode == ViewMode::BottomUp) {
		offset = glm::vec3(0.0f, -BOTTOM_UP_DISTANCE, 0.0f);
		drone_camera->SetVectors(drone_pos + offset, glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));
		player_drone->SetScale(DRONE_ARROW_INDEX, glm::vec3(1.0f));
	}
}

