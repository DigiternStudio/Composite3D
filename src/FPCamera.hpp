#ifndef CM3D_FIRST_PERSON_CAMERA_HPP
#define CM3D_FIRST_PERSON_CAMERA_HPP

/*===========================================================
 * First Person Camera for default rendering with
 * perspective view frustum. Supports changing world-up axis.
 */

#include "cmcommon.h"
#include <glm/gtc/type_ptr.hpp>

NAMESPACE_BEGIN(cm3d)

typedef void (*FPCamera)

enum class FPCameraMovDirection
{
	FORWARD =	0x1,
	BACKWARD =	0x2,
	LEFT =		0x4,
	RIGHT =		0x8,
	W_UP =		0x10,
	W_DOWN =	0x20,
	REL_UP =	0x40,
	REL_DOWN =	0x80
};

class FPCamera
{
public:
	float FoV; // radians
	float renderDist;
	float offsetRsp; // response to mouse offset
	float accFactor; // acceleration
	float dampFactor; // damping
	
	glm::dvec3 worldPos;
	glm::dvec3 worldVel;
	
	FPCameraMode viewMode;
	
	FPCamera
	(
		glm::dvec3 wPos = glm::dvec3(0.0), glm::dvec3 wVel = glm::dvec3(0.0),
		glm::vec3 forwardDir = glm::vec3(0.f, 0.f, 1.f),
		glm::vec3 upDir = glm::vec3(0.f, 1.f, 0.f),
		float FoV = 1.57f, float renderDist = 300.f, float offsetReponse = 0.017f,
		float acceleration = 40.f, float damping = 4.f
	);
	
	glm::mat4 viewMatrix(float aspect);
	
	void applyVelDelta(const int direction, const double dTime);
	// Default is mouse callback
	void offsetCallback(const glm::vec2 offset);
	void scrollCallback(const float yoffset, const int mods);
	
	void processPhysics(const double dTime);
	void recalculateDirections();
	
	float verticalAngle; // angle between unitUpAbsolute & unitForward
	glm::vec3 unitUp; // current up direction used to calculate lookAt
	glm::vec3 unitForward;
	glm::vec3 unitLeft; // cache (= glm::cross(unitUp, unitForward))

protected:
	glm::vec3 unitUpAbsolute;
	
	friend class Overlay;
}

NAMESPACE_END(cm3d)

#endif // FIRST_PERSON_CAMERA_HPP