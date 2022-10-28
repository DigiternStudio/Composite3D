#include <FPCamera.hpp>
#include <localMath.hpp>
#include <CelestialBody.hpp>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

NAMESPACE_BEGIN(cm3d)

FPCamera::FPCamera
(
	glm::dvec3 wPos, glm::dvec3 wVel,
	glm::vec3 forwardDir, glm::vec3 upDir,
	float FoV, float renderDist,
	float accFactor, float dampFactor, float offsetResponse
):
	worldPos(wPos), worldVel(wVel),
	unitForward(forwardDir), unitUp(upDir),
	FoV(FoV), renderDist(renderDist), 
	accFactor(accFactor), dmpFactor(dmpFactor), offsetRsp(offsetResponse)
{
	verticalAngle = std::acos(glm::dot(upDir, forwardDir));
	unitUpAbsolute = upDir;
	
	recalculateDirections();
}

glm::mat4 FPCamera::viewMatrix(float aspect)
{
	glm::vec3 w_pos = glm::vec3(worldPos);
	glm::mat4 look_at = glm::lookAt(w_pos, w_pos + unitForward, unitUp);
	return glm::perspective(FoV, aspect, 0.1f, renderDist) * look_at;
}

void FPCamera::applyVelDelta(const int direction, const double dTime)
{
	glm::vec3 mov = glm::vec3(0.f);
	
	if (direction & (FPCAM_MOV_FW | FPCAM_MOV_BW))
	{
		mov = glm::normalize(glm::cross(unitUpAbsolute, unitRight));
		if (direction & FPCAM_MOV_BW) mov = -mov;
	}
	if (direction & FPCAM_MOV_L)
		mov += unitLeft;
	else if (direction & FPCAM_MOV_R)
		mov -= unitLeft;
	if (direction & FPCAM_MOV_UP)
		mov += unitUpAbsolute;
	else if (direction & FPCAM_MOV_DN)
		mov -= unitUpAbsolute;
	
	if (mov != glm::vec3(0.f))
		wVel += glm::dvec3(glm::normalize(mov)) * (dTime * worldAcc);
}

void FPCamera::offsetCallback(const glm::vec2 offset);
{
	verticalAngle += offset.y * offsetRsp;
	if (verticalAngle > 3.14f)
		verticalAngle = 3.14f;
	else if (verticalAngle < 0.01f)
		verticalAngle = 0.01f;
	
	if (offset.x != 0.0f)
	{
		glm::mat3 ryaw = glm::mat3(glm::rotate(glm::mat4(1.f), offset.x * offsetRsp, unitUpAbsolute));
		unitLeft = glm::vec3(ryaw * unitLeft);
		unitForward = glm::vec3(ryaw * unitForward);
	}
	recalculateDirections();
}

void FPCamera::scrollCallback(const float yoffset, const int glfwMods)
{
	if (glfwMods & GLFW_MOD_ALT)
	{
		FoV -= yoffset * 0.03f;
		if (FoV < 0.07f)
			FoV = 0.07f;
		else if (FoV > 2.1f)
			FoV = 2.1f;
	}
	else {
		renderDist += 25.f * yoffset;
		if (renderDist < 25.f)
			renderDist = 25.f;
	}
}

void FPCamera::processPhysics(const double dTime)
{
	if (wVelocity != glm::dvec3(0.0))
	{
		glm::dvec3 vel = worldVel;
		worldVel -= glm::normalize(worldVel) * (worldDmp * dTime);
		if (glm::dot(vel, wVelocity) < 0.3)
			worldVel = glm::dvec3(0.0);
		
		worldPos += worldVel * dTime;
	}
	recalculateDirections();
}

void FPCamera::recalculateDirections()
{
	if (viewMode == FPCameraMode::Align && wPos != bodyAlign->worldPos)
		unitUpAbsolute = glm::normalize(wPos - bodyAlign->worldPos);
	
	unitLeft = glm::normalize(glm::cross(unitUpAbsolute, unitForward));
	unitForward = lcm::rotate(unitUpAbsolute, verticalAngle, unitLeft);
}

NAMESPACE_END(cm3d)