#pragma once

#include "utils/glm_utils.h"


namespace transform3D
{
    // Translate matrix
    inline glm::mat4 Translate(glm::vec3 amount)
    {
        return glm::mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            amount.x, amount.y, amount.z, 1
        );
    }

    // Scale matrix
    inline glm::mat4 Scale(glm::vec3 amount)
    {
        return glm::mat4(
            amount.x, 0, 0, 0,
            0, amount.y, 0, 0,
            0, 0, amount.z, 0,
            0, 0, 0, 1
        );
    }

    // Rotate matrix relative to the OZ axis
    inline glm::mat4 RotateOZ(float radians)
    {
        return glm::mat4(
            cos(radians), -sin(radians), 0, 0,
            sin(radians), cos(radians), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    }

    inline glm::mat4 RotateOY(float radians)
    {
        return glm::mat4(
            cos(radians), 0, sin(radians), 0,
            0, 1, 0, 0,
            -sin(radians), 0, cos(radians), 0,
            0, 0, 0, 1
        );
    }

    inline glm::mat4 RotateOX(float radians)
    {
        return glm::mat4(
            1, 0, 0, 0,
            0, cos(radians), -sin(radians), 0,
            0, sin(radians), cos(radians), 0,
            0, 0, 0, 1
        );
    }

}   // namespace transform3D
