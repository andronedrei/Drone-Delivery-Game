#include "lab_m1/Tema2/utils/my_camera.h"
#include "lab_m1/Tema2/utils/constants.h"
#include "utils/glm_utils.h"

MyCamera::MyCamera()
{
	distanceToTarget = THIRD_PERSON_DISTANCE;
	horizontalAngle = 0.0f;
	verticalAngle = 20.0f;

	targetPosition = glm::vec3(0, 0, 0);
	position = glm::vec3(0, 0, 0);
}

void MyCamera::InitThirdPerson(const glm::vec3& target, float distance, float initialHorAngle, float initialVertAngle)
{
	targetPosition = target;
	distanceToTarget = distance;
	horizontalAngle = initialHorAngle;
	verticalAngle = initialVertAngle;

	UpdateCameraPositionThirdPerson();
}

MyCamera::~MyCamera() {}

void MyCamera::SetPos(const glm::vec3& pos)
{
	position = pos;
}

void MyCamera::SetTarget(const glm::vec3& target)
{
	targetPosition = target;
}

// Update camera
void MyCamera::Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
{
	this->position = position;
	forward = glm::normalize(center - position);
	right = glm::cross(forward, up);
	this->up = glm::cross(right, forward);
}

void MyCamera::SetVectors(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up)
{
	this->position = position;
	this->forward = glm::normalize(forward);
	this->up = glm::normalize(up);
	this->right = glm::cross(this->forward, this->up);
}

void MyCamera::GetCameraData(glm::vec3& position, glm::vec3& forward, glm::vec3& up)
{
	position = this->position;
	forward = this->forward;
	up = this->up;
}

void MyCamera::MoveForward(float distance)
{
	// Translates the camera using the `dir` vector computed from
	// `forward`. Movement will always keep the camera at the same
	// height. For example, if you rotate your head up/down, and then
	// walk forward, then you will still keep the same relative
	// distance (height) to the ground!
	glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
	position += dir * distance;
}

void MyCamera::TranslateForward(float distance)
{
	// Translate the camera using the `forward` vector.
	glm::vec3 dir = glm::normalize(glm::vec3(forward.x, forward.y, forward.z));
	position += dir * distance;
}

void MyCamera::TranslateUpward(float distance)
{
	// TODO(student): Translate the camera using the `up` vector.
	glm::vec3 dir = glm::normalize(glm::vec3(up.x, up.y, up.z));
	position += dir * distance;
}

void MyCamera::TranslateRight(float distance)
{
	// TODO(student): See instructions below. Read the entire thing!
	// You need to translate the camera using the `right` vector.
	// Usually, however, translation using camera's `right` vector
	// is not very useful, because if the camera is rotated around the
	// `forward` vector, then the translation on the `right` direction
	// will have an undesired effect, more precisely, the camera will
	// get closer or farther from the ground. The solution is to
	// actually use the projected `right` vector (projected onto the
	// ground plane), which makes more sense because we will keep the
	// same distance from the ground plane.

	glm::vec3 projectedRight = glm::normalize(glm::vec3(right.x, 0, right.z));
	position += projectedRight * distance;
}

void MyCamera::RotateFirstPerson_OX(float angle)
{
	// Compute the new `forward` and `up` vectors.
	
	glm::vec4 dir = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(forward, 1);
	forward = glm::normalize(glm::vec3(dir));

	glm::vec4 dir1 = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(up, 1);
	up = glm::normalize(glm::vec3(dir1));
}

void MyCamera::RotateFirstPerson_OY(float angle)
{
	// Compute the new `forward`, `up` and `right` vectors. 

	glm::vec4 newVector = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1);
	forward = glm::normalize(glm::vec3(newVector));

	glm::vec4 newVector1 = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(up, 1);
	up = glm::normalize(glm::vec3(newVector1));

	glm::vec4 newVector2 = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1);
	right = glm::normalize(glm::vec3(newVector2));
}

void MyCamera::RotateFirstPerson_OZ(float angle)
{
	// Compute the new `right` and `up`. This time,
	// `forward` stays the same

	glm::vec4 newVector = glm::rotate(glm::mat4(1.0f), angle, forward) * glm::vec4(up, 1);
	up = glm::normalize(glm::vec3(newVector));

	glm::vec4 newVector1 = glm::rotate(glm::mat4(1.0f), angle, forward) * glm::vec4(right, 1);
	right = glm::normalize(glm::vec3(newVector1));

}

void MyCamera::RotateThirdPerson_OX(float angle)
{
	verticalAngle += glm::degrees(angle);

	if (verticalAngle > maxVerticalAngle) verticalAngle = maxVerticalAngle;
	if (verticalAngle < minVerticalAngle) verticalAngle = minVerticalAngle;
}

void MyCamera::RotateThirdPerson_OY(float angle)
{
	horizontalAngle += glm::degrees(angle);

	// mentinem unghiul 0-360
	if (horizontalAngle > 360.0f) horizontalAngle -= 360.0f;
	if (horizontalAngle < 0.0f) horizontalAngle += 360.0f;
}

void MyCamera::RotateThirdPerson_OZ(float angle)
{
	// nu folosim
}

glm::mat4 MyCamera::GetViewMatrix() const
{
	// Returns the view matrix
	return glm::lookAt(position, position + forward, up);
}

glm::vec3 MyCamera::GetTargetPosition() const
{
	return position + forward * distanceToTarget;
}

void MyCamera::UpdateCameraPositionThirdPerson()
{
	float radH = glm::radians(horizontalAngle);
	float radV = glm::radians(verticalAngle);

	// Calculam noua pozitie a camerei pe baza unghiurilor sferice
	position.x = targetPosition.x + distanceToTarget * cos(radV) * sin(radH);
	position.y = targetPosition.y + distanceToTarget * sin(radV);
	position.z = targetPosition.z + distanceToTarget * cos(radV) * cos(radH);

	forward = glm::normalize(targetPosition - position);
	right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
	up = glm::normalize(glm::cross(right, forward));
}