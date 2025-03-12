#pragma once

#include "utils/glm_utils.h"

const float GRAVITY = 9.81f;
const float NO_TRANSPARENCY = 1.0f;
const glm::vec3 ORIGIN = glm::vec3(0, 0, 0);

// PT DRONA
const int NUM_DRONE_BODY = 2;
const int NUM_DRONE_PROPELLER = 4;

const glm::vec3 DRONE_BODY_SIZE = glm::vec3(1.0f, 0.1f, 0.1f); // dim parte corp
const glm::vec3 DRONE_BODY_COLOR = glm::vec3(0.7f, 0.7f, 0.7f);
const glm::vec3 DRONE_SUPPORT_SIZE = glm::vec3(0.05f, 0.3f, 0.05f); // dim suport
const glm::vec3 DRONE_SUPPORT_COLOR = glm::vec3(0.7f, 0.7f, 0.7f);
const glm::vec3 DRONE_PROPELLER_SIZE = glm::vec3(0.05f, 0.05f, 0.2f); // dim elice
const glm::vec3 DRONE_PROPELLER_COLOR = glm::vec3(0, 0, 0);
const float PROPELLER_ROTATION_SPEED_FACTOR = 3.0f;
// modifica mai jos in functie de ce ai pus sus
const glm::vec3 COLLISION_DRONE_HALF_SIZE = glm::vec3(0.5f, 0.5f, 0.05f);
const float COLLISION_DRONE_RADIUS = 0.5f;

const int DRONE_ARROW_INDEX = 10;

// ELEMENTE FIZICA
const float MIN_THRUST = 6.0f;
const float THRUST_STEP = 4.0f;
const float MAX_THRUST = 14.0f;

const float MAX_ANGLE_DEVIATION = RADIANS(45.0f);
const float DRONE_ROTATION_SPEED = RADIANS(50.0f);

const float DRAG_COEFICIENT = 0.9f; // folosit pentru acceleratia fortelor rezistive

const float DRONE_TEST_SPEED = 30.0f;

// PT HARTA
const float MAP_SIZE = 350.0f; // dimensiunea hartii pe o directie
const float MAP_MARGINS = 50.0f;
const int MAP_GRANULARITY = 100;

// PT ARBORI
const int NUM_TREES = 75;
const int NUM_TREE_MESHES = 5;
const float MAX_TREE_RADIUS = 3.0f;
const float MAX_TREE_HEIGHT = 30.0f;
const float MAX_TREE_TRUNK_RATIO = 0.40f;
const glm::vec3 TREE_TRUNK_COLOR = glm::vec3(0.25f, 0.15f, 0.05f);
const glm::vec3 TREE_CROWN_COLOR = glm::vec3(0.2f, 0.7f, 0.4f);
const float CROWN_TRUNK_RADIUS_RATIO = 2.5f;

const float MAX_TREE_ROTATION = RADIANS(50.0f);
const float ROTATION_DISTANCE_TRESSHOLD = 15.0f;

// PT CLADIRI
const int NUM_BUILDINGS = 25;
const int NUM_BUILDING_MESHES = 2;
const float BUILDING_ROOF_RATIO = 0.3f;
const float MAX_BUILDING_HEIGHT = 20.0f;
const float MAX_BUILDING_WIDTH = 15.0f;
const float MAX_BUILDING_LENGTH = 12.0f;
const glm::vec3 BUILDING_BODY_COLOR = glm::vec3(0.2f, 0.2f, 0.2f);
const glm::vec3 BUILDING_ROOF_COLOR = glm::vec3(0.8f, 0.1f, 0.0f);

// PT TEREN
const float TERRAIN_NOISE_HEIGHT_FACTOR = 0.1f;
const float TERRAIN_MAX_HEIGHT = 1.0f;
const glm::vec3 TERRAIN_COLOR_MIX_1 = glm::vec3(0.3f, 0.2f, 0.1f); // maro mai închis
const glm::vec3 TERRAIN_COLOR_MIX_2 = glm::vec3(0.2f, 0.3f, 0.15f); // maro mai deschis

// PT CUTIA DE LIVRAREZ
const int DELIVERY_BOX_NUM_MESHES = 1;
const float DELIVERY_BOX_SIZE = 2.0f;
const float DELIVEERY_BOX_COLLISION_RADIUS = 3.0f;
const glm::vec3 DELIVERY_BOX_COLOR = glm::vec3(0.2f, 0.3f, 0.5f);
const int BOX_INDEX = 11;

// PT FRANGIA 
const float DELIVERY_BOX_ROPE_LENGTH = 1.0f;
const float DELIVERY_BOX_ROPE_RADIUS = 0.02f;
const glm::vec3 DELIVERY_BOX_ROPE_COLOR = glm::vec3(0);
const int ROPE_INDEX = 12;
const float DELIVERY_BOX_HEIGHT_TRESSHOLD = 0.1f;

// PT ZONA DE LIVRARE
const int DELIVERY_AREA_NUM_MESHES = 1;
const float DELIVERY_AREA_RADIUS = 10.0f;
const float DELIVERY_AREA_HEIGHT = 50.0f;
const float DELIVERY_AREA_TRANSPARENCY = 0.5f;
const glm::vec3 DELIVERY_AREA_COLOR = glm::vec3(0.8f, 0.7f, 0.6f);
const float DELIVERY_AREA_ROTATION_SPEED = RADIANS(10.0f);

// PT SAGEATA DE DIRECTIE
const float ARROW_RADIUS = 2.5f;
const float ARROW_LENGTH = 9.0f;
const glm::vec3 ARROW_COLOR = glm::vec3(0.0f, 0.0f, 1.0f);
const float ARROW_TRANSPARENCY = 0.8f;

// PT CAMERA
const float DRONE_CAMERA_DISTANCE = 150.0f;
const float MAP_CAMERA_DISTANCE = 150.0f;
const float MAP_CAMERA_SIZE = 350.0f;
const glm::vec3 TEMP_MAP_DRONE_SCALE = glm::vec3(20.0f, 20.0f, 20.0f);
const glm::vec3 TEMP_MAP_BOX_SCALE = glm::vec3(4.0f, 4.0f, 4.0f);
const float DRONE_CAMERA_ANGLE = RADIANS(65.0f);

const float FIRST_PERSON_DISTANCE_RATIO = 0.05f;   
const float THIRD_PERSON_DISTANCE = 4.0f;
const float THIRD_PERSON_HEIGHT_OFFSET = 2.0f;
const float BOTTOM_UP_DISTANCE = 3.0f;

