#pragma once

#include "utils/glm_utils.h"

#include "lab_m1/Tema2/utils/my_camera.h"
#include "lab_m1/Tema2/elements/screen_element.h"
#include "lab_m1/Tema2/elements/drone.h"
#include "lab_m1/Tema2/elements/simple_objects.h"

#include <vector>

const unsigned int NUM_DELIVERY_STATES = 4;

class DeliveryBox : public ScreenElement {
  protected:
	float size;
	bool attached;

	float fall_speed;

  public:
    DeliveryBox();
    ~DeliveryBox();

    void InitCollideData() override;

    void SetPosition(glm::vec3 position);
    bool IsAttached();
	void SetAttached(bool attached);
	float GetSize();

	void PrepareFall();
	void Fall(float deltaTimeSeconds);
};

class DeliveryArea : public ScreenElement {
  protected:
	float radius;
	float height;

	
	glm::vec3 rotation_center;
	float rotation_radius;
	float rotation_speed;
	float rotation_angle;

	void InitCollideData() override;

  public:
	DeliveryArea();
	~DeliveryArea();

	void ResetRotation();
	void Rotate(float deltaTimeSeconds);

	void SetRotationCenter(glm::vec3 position);
	float GetRadius();
	float GetHeight();

	void RenderArea(Shader* shader, MyCamera* camera, glm::mat4& projectionMatrix);
};

class DirectionArrow : public ScreenElement {
  public:
	DirectionArrow();
	~DirectionArrow();

	void InitCollideData() override;

	void SetPosition(glm::vec3 position, bool atached);
	void SetObjective(glm::vec3 objective_pos);

	void RenderArrow(Shader* shader, MyCamera* camera, glm::mat4& projectionMatrix);
};

enum DeliveryState {
	GENERATING,
	DELIVERING,
	WAITING_DELIVERY,
};

class DeliveryManager {
  protected:
	DeliveryBox box;
	DeliveryArea area;
	DirectionArrow arrow;

	Drone* drone; // referinta la drona controlata de player
	RandomPositionHandler* handler; // referinta la handler de pozitii random

	DeliveryState delivery_state; // starea curenta a livrarii
	int score;


	bool CheckAttatchDelivery(); // verifica daca putem atasa cutia
	bool CheckDelivery(); // verifica daca am livrat cutia

  public:
	DeliveryManager();
	~DeliveryManager();

	void Init(Drone* drone, RandomPositionHandler* handler);
	void Update(float deltaTimeSeconds);
	void PrepareAndRender(Shader* shader, MyCamera* camera, glm::mat4& projectionMatrix);

	void ReleaseBox();

	DeliveryBox* GetBox();
};