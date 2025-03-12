#pragma once

#include "lab_m1/Tema2/elements/screen_element.h"
#include "lab_m1/Tema2/utils/my_camera.h"
#include "lab_m1/Tema2/utils/constants.h"

#include "lab_m1/Tema2/elements/simple_objects.h"

class Tree;
class Building;

class Drone : public ScreenElement
{
  protected:
	bool player_controlled;
	bool delivery_attached;

	float thrust_power;
	float rotation_speed;

	glm::vec3 speed_vector;

	float old_deviation_angle;

	void InitCollideData() override;

	bool CheckCollisions(std::vector<Tree*>& trees, std::vector<Building*>& buildings);

	void RotateProppellers(float deltaTimeSeconds);
	void SlopeTrees(std::vector<Tree*>& trees);

	// Functie de miscat drona pe baza unor factori de fizica
	void Move(float deltaTimeSeconds);

	bool CheckOkRotation();

  public:
	Drone();
	~Drone();

	void Init(bool player_controlled);

	void Update(float deltaTimeSeconds, std::vector<Tree*>& trees, std::vector<Building*>& buildings);

	void SetAttached(bool attached);

	// Functii de miscare drona
	void IncreaseThrust(float deltaTimeSeconds);
	void DecreaseThrust(float deltaTimeSeconds);

	void IncreasePitch(float deltaTimeSeconds);
	void DecreasePitch(float deltaTimeSeconds);

	void IncreaseYaw(float deltaTimeSeconds);
	void DecreaseYaw(float deltaTimeSeconds);

	void IncreaseRoll(float deltaTimeSeconds);
	void DecreaseRoll(float deltaTimeSeconds);

	// Pt test
	void MakeDroneStraight();
	void TestTranslateForward(float deltaTimeSeconds);
	void TestTranslateBackward(float deltaTimeSeconds);
	void TestRotateRight(float deltaTimeSeconds);
	void TestRotateLeft(float deltaTimeSeconds);
	void TestTranslateUp(float deltaTimeSeconds);
	void TestTranslateDown(float deltaTimeSeconds);
};
