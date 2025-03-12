#include "lab_m1/Tema2/elements/delivery.h"
#include "lab_m1/Tema2/elements/drone.h"
#include "lab_m1/Tema2/elements/screen_element.h"

#include "lab_m1/Tema2/utils/constants.h"
#include "lab_m1/Tema2/utils/functions.h"

#include <vector>
#include <iostream>

void DeliveryBox::InitCollideData() {
	int nr = 1;
	collide_data.num_collide_boxes = nr;

	collide_data.collide_box_center_offset = new glm::vec3[nr];
	collide_data.half_size = new glm::vec3[nr];

	collide_data.collide_box_center_offset[0] = ORIGIN;
	collide_data.half_size[0] = glm::vec3(size / 2.0f);

	collide_data.collide_sphere_center_offset = ORIGIN;
	collide_data.radius = DELIVEERY_BOX_COLLISION_RADIUS;
}

DeliveryBox::DeliveryBox() {
	size = DELIVERY_BOX_SIZE;
	attached = false;
	fall_speed = 0;

	ScreenElement::Init(DELIVERY_BOX_NUM_MESHES);

	meshes[0] = CreeateParallelepiped("delivery_box", ORIGIN, glm::vec3(size), DELIVERY_BOX_COLOR);
	world_pos = ORIGIN;

	InitCollideData();
}

DeliveryBox::~DeliveryBox() {}

void DeliveryBox::SetPosition(glm::vec3 position) {
	world_pos = position;
}

bool DeliveryBox::IsAttached() {
	return attached;
}

void DeliveryBox::SetAttached(bool attached) {
	this->attached = attached;

	if (!attached) {
		SetScale(ALL, glm::vec3(1.0f));
	} else {
		SetScale(ALL, glm::vec3(0.0f));
	}
}

float DeliveryBox::GetSize() {
	return size;
}

void DeliveryBox::PrepareFall() {
	fall_speed = 0;
}

void DeliveryBox::Fall(float deltaTimeSeconds) {
	fall_speed += GRAVITY * deltaTimeSeconds;
	world_pos.y -= fall_speed * deltaTimeSeconds;

	if (world_pos.y < size / 2.0f) {
		world_pos.y = size / 2.0f;
	}
}

void DeliveryArea::InitCollideData() {
	// NU FOLOSIM METODELE UZUALE DE COLLIDE
}

DeliveryArea::DeliveryArea() {
	radius = DELIVERY_AREA_RADIUS;
	height = DELIVERY_AREA_HEIGHT;

	ScreenElement::Init(DELIVERY_AREA_NUM_MESHES);

	meshes[0] = CreateCylinder("delivery_area", ORIGIN, radius, height, 20, DELIVERY_AREA_COLOR);
	world_pos = ORIGIN;

	rotation_speed = DELIVERY_AREA_ROTATION_SPEED;
	rotation_angle = 0;
	rotation_radius = 2.0f * radius;

	InitCollideData();
}

DeliveryArea::~DeliveryArea() {}

void DeliveryArea::ResetRotation() {
	rotation_angle = 0;
}

void DeliveryArea::Rotate(float deltaTimeSeconds) {
	rotation_angle += rotation_speed * deltaTimeSeconds;
	world_pos = rotation_center;

	world_pos.x += rotation_radius * cos(rotation_angle);
	world_pos.z += rotation_radius * sin(rotation_angle);
}


void DeliveryArea::SetRotationCenter(glm::vec3 position) {
	rotation_center = position;
}

float DeliveryArea::GetRadius() {
	return radius;
}

float DeliveryArea::GetHeight() {
	return height;
}

void DeliveryArea::RenderArea(Shader* shader, MyCamera* camera, glm::mat4& projectionMatrix) {
	MyRenderMesh(meshes[0], DELIVERY_AREA_TRANSPARENCY, shader, camera, projectionMatrix, mesh_data[0].model_mat);
}

void DirectionArrow::InitCollideData() {
	// NU FOLOSIM METODELE UZUALE DE COLLIDE
}

DirectionArrow::DirectionArrow() {
	ScreenElement::Init(1);

	// varf sageata
	meshes[0] = CreateCone("arrow_head", ORIGIN, ARROW_RADIUS, ARROW_LENGTH, 20, ARROW_COLOR);
	RotateOX(0, RADIANS(90.0f));

	world_pos = ORIGIN;

	InitCollideData();
}

DirectionArrow::~DirectionArrow() {}

void DirectionArrow::SetPosition(glm::vec3 position, bool atached) {
	world_pos = position;

	world_pos.y = DELIVERY_BOX_SIZE;

	// daca drona e aproape de nivelul solului sau pacehtul a fost ridicat nu afisam sageata sa nu incurcam
	if (position.y < DELIVERY_BOX_SIZE + ARROW_RADIUS || atached) {
		SetScale(ALL, glm::vec3(0.0f));
	}
	else {
		SetScale(ALL, glm::vec3(1.0f));
	}
}

void DirectionArrow::SetObjective(glm::vec3 objective_pos) {
	glm::vec3 direction = -objective_pos + world_pos;

	float angle = atan2(direction.x, direction.z);
	SetObjectAngleY(-angle); // setam directia sageata
	all_rotations_mat = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0));

	// daca e prea aproape de cutie, nu o afisam sa nu incurce obiectul
	if (glm::length(direction) < ARROW_LENGTH * 1.1f) {
		Hide();
	} else {
		Show();
	}
}

void DirectionArrow::RenderArrow(Shader* shader, MyCamera* camera, glm::mat4& projectionMatrix) {
	MyRenderMesh(meshes[0], ARROW_TRANSPARENCY, shader, camera, projectionMatrix, mesh_data[0].model_mat);
}

DeliveryManager::DeliveryManager() {}
DeliveryManager::~DeliveryManager() {}

void DeliveryManager::Init(Drone* drone, RandomPositionHandler* handler) {
	this->drone = drone;
	this->handler = handler;
	delivery_state = DeliveryState::GENERATING;
	score = 0;
}

void DeliveryManager::PrepareAndRender(Shader* shader, MyCamera* camera, glm::mat4& projectionMatrix) {
	box.PrepareData();
	area.PrepareData();
	arrow.PrepareData();

	box.Render(shader, camera, projectionMatrix);
	area.RenderArea(shader, camera, projectionMatrix);
	arrow.RenderArrow(shader, camera, projectionMatrix);
}

bool DeliveryManager::CheckAttatchDelivery() {
	if (!box.IsAttached() && drone->CheckCollision(&box)) {
		box.SetAttached(true);
		drone->SetAttached(true);
		return true;
	}

	return false;
}

void DeliveryManager::ReleaseBox() {
	box.SetAttached(false);
	drone->SetAttached(false);
}

bool DeliveryManager::CheckDelivery() {
	glm::vec3 area_center = area.GetWorldPos();
	glm::vec3 box_center = box.GetWorldPos();
	float box_size = box.GetSize();

	// verificam daca cutia este la nivelul solului
	if (box_center.y > box_size / 2.0f + DELIVERY_BOX_HEIGHT_TRESSHOLD) {	
		return false;
	}

	box_center.y = 0;
	if (!box.IsAttached() && glm::distance(area_center, box_center) < area.GetRadius()) {
		return true;
	}

	return false;
}

void DeliveryManager::Update(float deltaTimeSeconds) {

	area.Rotate(deltaTimeSeconds);
	arrow.SetPosition(drone->GetWorldPos(), box.IsAttached());
	arrow.SetObjective(box.GetWorldPos());

	switch (delivery_state) {
		case GENERATING:
			std::cout << "NEW DELIVERY TO PICK, HURRY UP !" << std::endl;

			box.SetPosition(handler->GetRandomPosition());
			area.SetRotationCenter(handler->GetRandomPosition());

			delivery_state = DeliveryState::WAITING_DELIVERY;
			area.ResetRotation();
			box.PrepareFall();
			break;

		case DELIVERING:
			glm::vec3 offset = glm::vec3(0, DELIVERY_BOX_SIZE + DELIVERY_BOX_ROPE_LENGTH, 0);
			box.SetPosition(drone->GetWorldPos() - offset);

			if (!box.IsAttached()) {
				delivery_state = DeliveryState::WAITING_DELIVERY;
				box.PrepareFall();
			}
			break;

		case WAITING_DELIVERY:
			box.Fall(deltaTimeSeconds);

			if (CheckAttatchDelivery()) {
				delivery_state = DeliveryState::DELIVERING;
			}

			if (CheckDelivery()) {
				std::cout << "DELIVERY DONE\n" << "CURRENT SCORE: " << ++score << "\n" << std::endl;
				delivery_state = DeliveryState::GENERATING;
			}
			break;
	}
}

DeliveryBox* DeliveryManager::GetBox() {
	return &box;
}