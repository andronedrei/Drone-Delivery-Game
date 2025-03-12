#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "utils/glm_utils.h"

#include "lab_m1/Tema2/utils/constants.h"
#include "lab_m1/Tema2/utils/functions.h"
#include "lab_m1/Tema2/elements/drone.h"

#include "vector"
#include <iostream>

Drone::Drone() {}
Drone::~Drone() {}

void Drone::Init(bool player_controlled) {
	this->player_controlled = player_controlled;
	thrust_power = GRAVITY;
	rotation_speed = DRONE_ROTATION_SPEED;
	speed_vector = glm::vec3(0);

	// initializam unghiurile
	old_deviation_angle = 0;

	ScreenElement::Init(NUM_DRONE_BODY + 2 * NUM_DRONE_PROPELLER + 1 + 2); // unul pentru directie si unul pentru cutie si franghie

	// adaugam 4 propelere care se invart si 4 suporturi pentru acestea
	meshes[0] = CreeateParallelepiped("drone_support_0", ORIGIN, DRONE_SUPPORT_SIZE, DRONE_SUPPORT_COLOR);
	meshes[1] = CreeateParallelepiped("drone_support_1", ORIGIN, DRONE_SUPPORT_SIZE, DRONE_SUPPORT_COLOR);
	meshes[2] = CreeateParallelepiped("drone_support_2", ORIGIN, DRONE_SUPPORT_SIZE, DRONE_SUPPORT_COLOR);
	meshes[3] = CreeateParallelepiped("drone_support_3", ORIGIN, DRONE_SUPPORT_SIZE, DRONE_SUPPORT_COLOR);
 
	Translate(0, glm::vec3(DRONE_BODY_SIZE.x * cos(RADIANS(45.0f)), DRONE_SUPPORT_SIZE.y, DRONE_BODY_SIZE.x * sin(RADIANS(45.0f))) * 0.4f);
	Translate(1, glm::vec3(DRONE_BODY_SIZE.x * cos(RADIANS(-45.0f)), DRONE_SUPPORT_SIZE.y, DRONE_BODY_SIZE.x * sin(RADIANS(-45.0f))) * 0.4f);
	Translate(2, glm::vec3(DRONE_BODY_SIZE.x * cos(RADIANS(135.0f)), DRONE_SUPPORT_SIZE.y, DRONE_BODY_SIZE.x * sin(RADIANS(135.0f))) * 0.4f);
	Translate(3, glm::vec3(DRONE_BODY_SIZE.x * cos(RADIANS(-135.0f)), DRONE_SUPPORT_SIZE.y, DRONE_BODY_SIZE.x * sin(RADIANS(-135.0f))) * 0.4f);

	meshes[4] = CreeateParallelepiped("drone_propeller_0", ORIGIN, DRONE_PROPELLER_SIZE, DRONE_PROPELLER_COLOR);
	meshes[5] = CreeateParallelepiped("drone_propeller_1", ORIGIN, DRONE_PROPELLER_SIZE, DRONE_PROPELLER_COLOR);
	meshes[6] = CreeateParallelepiped("drone_propeller_2", ORIGIN, DRONE_PROPELLER_SIZE, DRONE_PROPELLER_COLOR);
	meshes[7] = CreeateParallelepiped("drone_propeller_3", ORIGIN, DRONE_PROPELLER_SIZE, DRONE_PROPELLER_COLOR);

	Translate(4, glm::vec3(DRONE_BODY_SIZE.x * cos(RADIANS(45.0f)), DRONE_SUPPORT_SIZE.y * 2.5f, DRONE_BODY_SIZE.x * sin(RADIANS(45.0f))) * 0.4f);
	Translate(5, glm::vec3(DRONE_BODY_SIZE.x * cos(RADIANS(-45.0f)), DRONE_SUPPORT_SIZE.y * 2.5f, DRONE_BODY_SIZE.x * sin(RADIANS(-45.0f))) * 0.4f);
	Translate(6, glm::vec3(DRONE_BODY_SIZE.x * cos(RADIANS(135.0f)), DRONE_SUPPORT_SIZE.y * 2.5f, DRONE_BODY_SIZE.x * sin(RADIANS(135.0f))) * 0.4f);
	Translate(7, glm::vec3(DRONE_BODY_SIZE.x * cos(RADIANS(-135.0f)), DRONE_SUPPORT_SIZE.y * 2.5f, DRONE_BODY_SIZE.x * sin(RADIANS(-135.0f))) * 0.4f);

	// formam drone din 2 paralelipipede in forma de X
	meshes[8] = CreeateParallelepiped("drone_body_0", ORIGIN, DRONE_BODY_SIZE, DRONE_BODY_COLOR);
	meshes[9] = CreeateParallelepiped("drone_body_1", ORIGIN, DRONE_BODY_SIZE, DRONE_BODY_COLOR);

	RotateOY(8, RADIANS(45.0f));
	RotateOY(9, RADIANS(-45.0f));

	// adaugam un con care sa indice directia dronei
	meshes[10] = CreateCone("drone_direction", ORIGIN, 0.1f, 0.5f, 30, glm::vec3(0.5f, 0, 0));
	// il culcam sa semene cu o sageata
	RotateOX(10, RADIANS(90.0f));
	// il translatam putin main in fata
	Translate(10, glm::vec3(0, 0, -0.5f));

	// adaugam o cutie de livrare
	meshes[11] = CreeateParallelepiped("delivery_box", ORIGIN, glm::vec3(DELIVERY_BOX_SIZE), DELIVERY_BOX_COLOR);
	// adaugam o franghie
	meshes[12] = CreateCylinder("rope", ORIGIN, DELIVERY_BOX_ROPE_RADIUS, DELIVERY_BOX_ROPE_LENGTH, 20, DELIVERY_BOX_ROPE_COLOR);

	float offset = DELIVERY_BOX_ROPE_LENGTH / 2.0f;
	// translatam sub drona franghia
	Translate(12, glm::vec3(0, -offset, 0));

	// translatam cutia de livrare sub franghie
	offset += DELIVERY_BOX_SIZE / 2.0f;
	Translate(11, glm::vec3(0, -offset, 0));

	delivery_attached = false;

	// initializam datele de coliziune
	InitCollideData();

	world_pos = ORIGIN;
	world_pos.y = MAX_TREE_HEIGHT * 1.5f;
}

void Drone::InitCollideData() {
	int nr = 1;
	collide_data.num_collide_boxes = nr;

	collide_data.half_size = new glm::vec3[nr];
	collide_data.collide_box_center_offset = new glm::vec3[nr];

	collide_data.half_size[0] = COLLISION_DRONE_HALF_SIZE;
	collide_data.collide_box_center_offset[0] = ORIGIN;

	collide_data.collide_sphere_center_offset = ORIGIN;
	collide_data.radius = COLLISION_DRONE_RADIUS;
}

void Drone::RotateProppellers(float deltaTimeSeconds) {
	for (int i = NUM_DRONE_PROPELLER; i < NUM_DRONE_PROPELLER * 2; i++) {
		RotateOY(i, deltaTimeSeconds * thrust_power * PROPELLER_ROTATION_SPEED_FACTOR);
	}
}

void Drone::Move(float deltaTimeSeconds) {
	glm::vec3 acceleration;

	acceleration = thrust_power * up_dir;
	acceleration.y -= GRAVITY;
	acceleration -= DRAG_COEFICIENT * speed_vector;
	
	speed_vector += acceleration * deltaTimeSeconds;

	world_pos += speed_vector * deltaTimeSeconds;

	// blocam drona in interiorul hartii (pe toate cele 3 axe)
	if (world_pos.y < DELIVERY_BOX_SIZE) {
		world_pos.y = DELIVERY_BOX_SIZE;
		speed_vector.y = 0;
	}

	if (world_pos.y > 0.9f * DRONE_CAMERA_DISTANCE) {
		world_pos.y = 0.9f * DRONE_CAMERA_DISTANCE; // folosim aceasta valoare pentru a mentine vizibilitatea
		speed_vector.y = 0;
	}

	float treeshold = MAP_SIZE / 2.0f;

	if (world_pos.x < -treeshold) {
		world_pos.x = -treeshold;
		speed_vector.x = 0;
	}

	if (world_pos.x > treeshold) {
		world_pos.x = treeshold;
		speed_vector.x = 0;
	}

	if (world_pos.z < -treeshold) {
		world_pos.z = -treeshold;
		speed_vector.z = 0;
	}

	if (world_pos.z > treeshold) {
		world_pos.z = treeshold;
		speed_vector.z = 0;
	}
}

void Drone::Update(float deltaTimeSeconds, std::vector<Tree*>& trees, std::vector<Building*>& buildings) {
	RotateProppellers(deltaTimeSeconds);
	SlopeTrees(trees);

	if (delivery_attached) {
		SetScale(BOX_INDEX, glm::vec3(1.0f));
		SetScale(ROPE_INDEX, glm::vec3(1.0f));
	}
	else {
		SetScale(BOX_INDEX, glm::vec3(0.0f));
		SetScale(ROPE_INDEX, glm::vec3(0.0f));
	}

	Move(deltaTimeSeconds);
	// randam dupa miscare sa obtinem un efect smooth
	PrepareData();

	if (CheckCollisions(trees, buildings)) {
		// daca am detectat o coliziune, anulam miscarea
		world_pos -= speed_vector * deltaTimeSeconds;

		// deasemenea, datorita conservarii impulsului, inversam intr-o masura viteza
		speed_vector = -speed_vector * 0.75f;
		world_pos += speed_vector * deltaTimeSeconds;
	}
}

void Drone::SetAttached(bool attached) {
	this->delivery_attached = attached;
}

bool Drone::CheckCollisions(std::vector<Tree*>& trees, std::vector<Building*>& buildings) {
	for (int i = 0; i < trees.size(); i++) {
		if (CheckCollision(trees[i])) {
			return true;
		}
	}

	for (int i = 0; i < buildings.size(); i++) {
		if (buildings[i]->CheckPrecisePointCollision(world_pos)) {
			return true;
		}
	}

	return false;
}

void Drone::SlopeTrees(std::vector<Tree*>& trees) {
	for (int i = 0; i < trees.size(); i++) {
		float trunk_height = trees[i]->GetTrunkHeight();
		float crown_height = trees[i]->GetCrownHeight();

		glm::vec3 crown_center = trees[i]->GetWorldPos() + glm::vec3(0, trunk_height + crown_height / 2.0f, 0);
		glm::vec3 dist_v = crown_center - world_pos;
		float crown_part_height = crown_height / (NUM_TREE_MESHES - 1);

		float dist = glm::distance(world_pos, crown_center);
		if (dist > ROTATION_DISTANCE_TRESSHOLD) {
			// drona e prea departe
			continue;
		}

		glm::vec3 normalizezd_v = glm::normalize(dist_v);
		float shift_amount = (1 - dist / ROTATION_DISTANCE_TRESSHOLD) * MAX_TREE_ROTATION;

		glm::mat4 z_rot = glm::rotate(glm::mat4(1), -normalizezd_v.x * shift_amount, glm::vec3(0, 0, 1));
		glm::mat4 x_rot = glm::rotate(glm::mat4(1), normalizezd_v.z * shift_amount, glm::vec3(1, 0, 0));
		//trees[i]->SetObjectAngleZ(normalizezd_v.x * shift_amount);
		//trees[i]->SetObjectAngleX(-normalizezd_v.z * shift_amount);
		trees[i]->SetAllRotationsMat(x_rot * z_rot);
	}
}

void Drone::IncreaseThrust(float deltaTimeSeconds) {
	thrust_power += THRUST_STEP * deltaTimeSeconds;
	if (thrust_power > MAX_THRUST) {
		thrust_power = MAX_THRUST;
	}
}

void Drone::DecreaseThrust(float deltaTimeSeconds) {
	thrust_power -= THRUST_STEP * deltaTimeSeconds;
	if (thrust_power < MIN_THRUST) {
		thrust_power = MIN_THRUST;
	}
}

bool Drone::CheckOkRotation() {
	// calculam unghiul de deviatie intre "UpDir" si (0, 1, 0)
	float new_delta = glm::acos(glm::dot(up_dir, glm::vec3(0, 1, 0))); // in grade

	if (new_delta < MAX_ANGLE_DEVIATION || new_delta < old_deviation_angle) {
		old_deviation_angle = new_delta;
		return true;
	}

	return false;
}

void Drone::IncreasePitch(float deltaTimeSeconds) {
	RotateRightDir(deltaTimeSeconds * rotation_speed);
	if (!CheckOkRotation()) {
		RotateRightDir(-deltaTimeSeconds * rotation_speed);
	}
}

void Drone::DecreasePitch(float deltaTimeSeconds) {
	RotateRightDir(-deltaTimeSeconds * rotation_speed);
	if (!CheckOkRotation()) {
		RotateRightDir(deltaTimeSeconds * rotation_speed);
	}
}

void Drone::IncreaseYaw(float deltaTimeSeconds) {
	RotateOY(ALL, -deltaTimeSeconds * rotation_speed);
}

void Drone::DecreaseYaw(float deltaTimeSeconds) {
	RotateOY(ALL, deltaTimeSeconds * rotation_speed);
}

void Drone::IncreaseRoll(float deltaTimeSeconds) {
	RotateForwardDir(deltaTimeSeconds * rotation_speed);
	if (!CheckOkRotation()) {
		RotateForwardDir(-deltaTimeSeconds * rotation_speed);
	}
}

void Drone::DecreaseRoll(float deltaTimeSeconds) {
	RotateForwardDir(-deltaTimeSeconds * rotation_speed);
	if (!CheckOkRotation()) {
		RotateForwardDir(deltaTimeSeconds * rotation_speed);
	}
}

void Drone::MakeDroneStraight() {
	angle = glm::vec3(0);
	all_rotations_mat = glm::mat4(1);

	thrust_power = GRAVITY;
}

void Drone::TestTranslateForward(float deltaTimeSeconds) {
	Translate(ALL, forward_dir * deltaTimeSeconds * DRONE_TEST_SPEED);
}

void Drone::TestTranslateBackward(float deltaTimeSeconds) {
	Translate(ALL, -forward_dir * deltaTimeSeconds * DRONE_TEST_SPEED);
}

void Drone::TestRotateRight(float deltaTimeSeconds) {
	RotateOY(ALL, deltaTimeSeconds * rotation_speed * 2.0f);
}

void Drone::TestRotateLeft(float deltaTimeSeconds) {
	RotateOY(ALL, -deltaTimeSeconds * rotation_speed * 2.0f);
}

void Drone::TestTranslateUp(float deltaTimeSeconds) {
	Translate(ALL, up_dir * deltaTimeSeconds * DRONE_TEST_SPEED);
}

void Drone::TestTranslateDown(float deltaTimeSeconds) {
	Translate(ALL, -up_dir * deltaTimeSeconds * DRONE_TEST_SPEED);
}
