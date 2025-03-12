#pragma once

#include "utils/glm_utils.h"

#include "lab_m1/Tema2/utils/functions.h"
#include "lab_m1/Tema2/elements/screen_element.h"

#include <vector>

class RandomPositionHandler {
  protected:
	int max_objects;
	int nr_options;
	std::vector<glm::vec2> positions;

  public:
	RandomPositionHandler(int max_objects);
	~RandomPositionHandler();
	
	glm::vec3 GetRandomPosition();

};

class Tree : public ScreenElement {
  protected:
	float radius;
	float trunk_height;
	float crown_height;

	void GenerateRandomSize();
	void InitCollideData() override;

  public:
	Tree();
	~Tree();
	void Init(RandomPositionHandler* handler);
	float GetRadius();

	float GetTrunkHeight();
	float GetCrownHeight();
};

class Building : public ScreenElement {
protected:
	float width;
	float height_body;
	float height_roof;
	float length;

	// acoperisul va fi o sfera
	float roof_radius;

	void GenerateRandomSize();
	void InitCollideData() override;

public:
	Building();
	~Building();
	void Init(RandomPositionHandler* handler);

	// verifica coliziunea cu un punct si salveaza in point o pozitie corecta in cazul coliziunii
	bool CheckPrecisePointCollision(glm::vec3 point);
};