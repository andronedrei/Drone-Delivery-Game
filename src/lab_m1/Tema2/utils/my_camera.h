#pragma once

#include "utils/glm_utils.h"
#include <glm/gtc/matrix_transform.hpp>

enum ViewMode {
    FirstPerson,
    ThirdPerson,
    BottomUp
};

class MyCamera {
private:
    float distanceToTarget;
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;

    glm::vec3 targetPosition;

    float horizontalAngle;
    float verticalAngle;

    const float minVerticalAngle = -89.0f;
    const float maxVerticalAngle = 89.0f;

public:
    MyCamera();
    ~MyCamera();

    void InitThirdPerson(const glm::vec3& target, float distance, float initialHorAngle, float initialVertAngle);
    void SetPos(const glm::vec3& position);
    void SetTarget(const glm::vec3& target);
    void Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);
    void SetVectors(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up);
    void GetCameraData(glm::vec3& position, glm::vec3& forward, glm::vec3& up);

    void MoveForward(float distance);
    void TranslateForward(float distance);
    void TranslateUpward(float distance);
    void TranslateRight(float distance);

	// Rotatii pentru first-person view
	void RotateFirstPerson_OX(float angle);
	void RotateFirstPerson_OY(float angle);
	void RotateFirstPerson_OZ(float angle);

	// Rotatii pentru third-person view
    void RotateThirdPerson_OX(float angle);
    void RotateThirdPerson_OY(float angle);
    void RotateThirdPerson_OZ(float angle);

    glm::mat4 GetViewMatrix() const;
    glm::vec3 GetTargetPosition() const;

	// Update la pozitia camerei in functie de distanta si unghiuri
    void UpdateCameraPositionThirdPerson();
};

