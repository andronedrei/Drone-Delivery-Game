#include "utils/glm_utils.h"
#include "lab_m1/Tema2/utils/constants.h"

#include "lab_m1/Tema2/utils/functions.h"
#include "lab_m1/Tema2/elements/screen_element.h"
#include "lab_m1/Tema2/elements/simple_objects.h"

#include <iostream>

RandomPositionHandler::RandomPositionHandler(int max_objects) {
	int nr_line_objects = floor(sqrt(max_objects * 5));
	this->max_objects = nr_line_objects * nr_line_objects;

	nr_options = this->max_objects; // nr de pozitii disponibile la un moment dat
	positions.resize(nr_options);

	float offset_step = (MAP_SIZE - MAP_MARGINS) / static_cast<float>(nr_line_objects);
	int half = nr_line_objects / 2;

	for (int i = 0; i < nr_line_objects; i++) {
		for (int j = 0; j < nr_line_objects; j++) {
			// Corectarea indicelui: înlocuiește max_objects cu nr_line_objects
			positions[i * nr_line_objects + j] = glm::vec2(
				(i - half) * offset_step,
				(j - half) * offset_step
			);
		}
	}
}

RandomPositionHandler::~RandomPositionHandler() {}

glm::vec3 RandomPositionHandler::GetRandomPosition() {
	if (nr_options == 0) {
		return ORIGIN;
	}
	int pos = abs(rand()) % (nr_options);

	glm::vec2 ret_aux = positions[pos];
	positions.erase(positions.begin() + pos);
	nr_options--;

	return glm::vec3(ret_aux.x, 0, ret_aux.y);
}

Tree::Tree() {
	radius = 0;
	trunk_height = 0;
	crown_height = 0;
}

Tree::~Tree() {}

void Tree::GenerateRandomSize() {
	float randomVal = static_cast<float>(rand()) / RAND_MAX; // [0, 1)
	radius = randomVal * (MAX_TREE_RADIUS / 2) + (MAX_TREE_RADIUS / 2);

	randomVal = static_cast<float>(rand()) / RAND_MAX;
	float ratio = randomVal * (MAX_TREE_TRUNK_RATIO / 2) + (MAX_TREE_TRUNK_RATIO / 2);

	randomVal = static_cast<float>(rand()) / RAND_MAX;
	float height = randomVal * (MAX_TREE_HEIGHT / 2) + (MAX_TREE_HEIGHT / 2);

	trunk_height = ratio * height;
	crown_height = height - trunk_height;
}

void Tree::Init(RandomPositionHandler* handler) {
	ScreenElement::Init(NUM_TREE_MESHES);
	GenerateRandomSize();
	world_pos = handler->GetRandomPosition();
	
	float offset;
	float crown_part_height = crown_height / (NUM_TREE_MESHES - 1);

	// Trunk
	meshes[0] = CreateCylinder("trunk", ORIGIN, radius, trunk_height, 20, TREE_TRUNK_COLOR);
	offset = trunk_height / 2;
	Translate(0, glm::vec3(0, offset, 0));

	// Crown
	offset = trunk_height;
	for (int i = 1; i < NUM_TREE_MESHES; i++) {
		meshes[i] = CreateCone("crown_part", ORIGIN, radius * CROWN_TRUNK_RADIUS_RATIO, crown_part_height, 20, TREE_CROWN_COLOR);
		Translate(i, glm::vec3(0, offset, 0));
		offset += crown_part_height / 2;
	}

	InitCollideData();
}

void Tree::InitCollideData() {
	float total_height = trunk_height + crown_height;
	int nr = 2; // una pentru trunchi si una pentru coroana

	collide_data.num_collide_boxes = nr; 
	collide_data.collide_box_center_offset = new glm::vec3[nr];
	collide_data.half_size = new glm::vec3[nr];

	// trunchi
	collide_data.half_size[0] = glm::vec3(radius / 1.44f, trunk_height / 2, radius / 1.44f);
	collide_data.collide_box_center_offset[0] = glm::vec3(0, trunk_height / 2, 0);

	// coroana
	float crown_radius = radius * CROWN_TRUNK_RADIUS_RATIO;
	float crown_collide_height = crown_height / 2 * 0.95f;
	collide_data.half_size[1] = glm::vec3(crown_radius / 1.44f, crown_collide_height / 2, crown_radius / 1.44f);
	collide_data.collide_box_center_offset[1] = glm::vec3(0, trunk_height + crown_collide_height / 2, 0);

	// aproximare grosiera
	collide_data.radius = total_height / 2;
	collide_data.collide_sphere_center_offset = glm::vec3(0, total_height / 2, 0);
}

float Tree::GetTrunkHeight() {
	return trunk_height;
}

float Tree::GetCrownHeight() {
	return crown_height;
}

float Tree::GetRadius() {
	return radius;
}

Building::Building() {
	width = 0;
	height_roof = 0;
	height_body = 0;
	length = 0;
}

Building::~Building() {}

void Building::GenerateRandomSize() {
	float randomVal = static_cast<float>(rand()) / RAND_MAX;
	width = randomVal * (MAX_BUILDING_WIDTH / 2) + (MAX_BUILDING_WIDTH / 2);

	randomVal = static_cast<float>(rand()) / RAND_MAX;
	float total_height = randomVal * (MAX_BUILDING_HEIGHT / 2) + (MAX_BUILDING_HEIGHT / 2);
	height_roof = total_height * BUILDING_ROOF_RATIO;
	height_body = total_height - height_roof;

	randomVal = static_cast<float>(rand()) / RAND_MAX;
	length = randomVal * (MAX_BUILDING_LENGTH / 2) + (MAX_BUILDING_LENGTH / 2);
}

void Building::Init(RandomPositionHandler* handler) {
	ScreenElement::Init(NUM_BUILDING_MESHES);
	GenerateRandomSize();
	world_pos = handler->GetRandomPosition();

	// Body
	meshes[0] = CreeateParallelepiped("body", ORIGIN, glm::vec3(width, height_body, length), BUILDING_BODY_COLOR);
	Translate(0, glm::vec3(0, height_body / 2, 0));

	// Roof
	//roof_radius = MAX(width, length) / 2;
	//meshes[1] = CreateCone("roof", ORIGIN, roof_radius, height_roof, 20, BUILDING_ROOF_COLOR);
	roof_radius = height_roof / 2;
	meshes[1] = CreateSphere("roof", ORIGIN, roof_radius, 20, 20, BUILDING_ROOF_COLOR);

	Translate(1, glm::vec3(0, height_body + roof_radius, 0));

	InitCollideData();
}

void Building::InitCollideData() {
	int nr = 2; // una pentru corp si una pentru acoperis
	collide_data.num_collide_boxes = nr;

	collide_data.collide_box_center_offset = new glm::vec3[nr];
	collide_data.half_size = new glm::vec3[nr];

	// pt corp
	collide_data.half_size[0] = glm::vec3(width / 2, height_body / 2, length / 2);
	collide_data.collide_box_center_offset[0] = glm::vec3(0, height_body / 2, 0);

	// pt acoperis
	collide_data.half_size[1] = glm::vec3(roof_radius, height_roof / 2, roof_radius);
	collide_data.collide_box_center_offset[1] = glm::vec3(0, height_body + height_roof / 2, 0);

	// aproximare grosiera
	collide_data.radius = height_body + height_roof;
	collide_data.collide_sphere_center_offset = (glm::vec3(0, (height_body + height_roof) / 2, 0));
}

bool Building::CheckPrecisePointCollision(glm::vec3 point) {
	glm::vec3 local_point = point - world_pos;

	// verificam colizunea cu paralelipipedul corpului
	if (abs(local_point.x) <= width / 2 && abs(local_point.y) <= height_body && abs(local_point.z) <= length / 2) {
		return true;
	}

	// verificam coliziunea cu sfera acoperisului
	glm::vec3 local_roof_center = glm::vec3(0, height_body + height_roof / 2, 0);
	if (glm::distance(local_point, local_roof_center) <= roof_radius) {
		return true;
	}

	return false;
}