#pragma once

#include <vector>

#include "components/simple_scene.h"

#include "lab_m1/Tema2/utils/my_camera.h"
#include "lab_m1/Tema2/elements/drone.h"
#include "lab_m1/Tema2/elements/terrain.h"
#include "lab_m1/Tema2/elements/delivery.h"
#include "lab_m1/Tema2/elements/simple_objects.h"


namespace m1
{
    class DroneScene : public gfxc::SimpleScene
    {
     public:
        DroneScene();
        ~DroneScene();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

    protected:
        Shader *defaultShader, *terrainShader;
		ViewMode view_mode;

        // date camera drona
		glm::mat4 projectionMatrix;
		MyCamera* drone_camera;

        // date camera harta
		glm::mat4 mapProjectionMatrix;
		MyCamera* map_camera;

        glm::vec3 aux_drone_camera_pos;
        glm::vec3 aux_drone_camera_forward;
		glm::vec3 aux_drone_camera_up;

		DroneTerrain* terrain;
		Drone* player_drone;
		RandomPositionHandler* handler;
		std::vector<Tree*> trees;
		std::vector<Building*> buildings;
		DeliveryManager* delivery_manager;

        void InitDroneView();
		void InitMapView();
		void UpdateMapView();
		void UpdateDroneView();

        void UpdateScreen();

		void RenderAllMeshes(MyCamera* camera, glm::mat4& projMatrix);

	    void SetViewMode(ViewMode mode);
    };
}   // namespace m1