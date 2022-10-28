#include <runtime/AppOverlay.hpp>
#include <runtime/World.hpp>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <stdexcept>

// TODO: callbacks for GLFW window

NAMESPACE_BEGIN(cm3d)

#if SBE_DEBUG
#  define OVERLAY_RUNTIME_ERROR_FUNC(expl) { throw std::runtime_error(expl); }
#else
#  define OVERLAY_RUNTIME_ERROR_FUNC(expl) { return; }
#endif

static int kMods = 0; // shift control alt etc
static int mState = 0; // LMB RMB

static unsigned long step_marker(unsigned long index);
static double step_clamp_linear(double val);
static double step_pow(double val);

Overlay::Overlay(World* world, int width, int height, const char *title, GLFWmonitor* monitor, GLFWwindow* share)
{
	if (initGLFW(monitor) != 0)
		OVERLAY_RUNTIME_ERROR_FUNC("Failed to init GLFW")
	
	if (!width)
		width = winWidth;
	if (!height)
		height = winHeight;
	
	context = glfwCreateWindow(width, height, title, monitor, share);
	if (context == NULL)
	{
		glfwTerminate();
		OVERLAY_RUNTIME_ERROR_FUNC("Failed to create GLFW window")
	}
	glfwMakeContextCurrent(wnd);
	if (loadGL() != 0)
	{
		glfwTerminate();
		OVERLAY_RUNTIME_ERROR_FUNC("Failed to load OpenGL")
	}
	linkWorld(world);
}

int Overlay::initGLFW(GLFWmonitor* monitor)
{
	if (!glfwInit())
		return -10021;
	
	if (monitor == NULL)
		monitor = glfwGetPrimaryMonitor();
	
	const GLFWvidmode* vm = glfwGetVideoMode(monitor);
	winWidth = vm->width;
	winHeight = vm->height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, vm->refreshRate);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

int Overlay::loadGL()
{
	if (!gladLoaderLoadGL())
	{
		glfwTerminate();
		return -10022;
	}

	// planetShader.compile("./shaders/planet_vs.glsl", "./shaders/planet_fs.glsl");
	stdShader.compile("./shaders/default_vs.glsl", "./shaders/default_fs.glsl");
	// mShader.compile("./shaders/marker_vs.glsl", "./shaders/marker_fs.glsl");
	
	// loadCubemap();

	// planetShader.use();
	// planetShader.setInt("worldmap", 0);

	glClearColor(0.678f, 0.784f, 0.898f, 1.f);
	glEnable(GL_DEPTH_TEST);
	viewChange = true;
}

void Overlay::linkWorld(World* world)
{
	this->linkedWorld = world;
	world->linkedOverlay = this;
}
	
void Overlay::linkCamera(Camera* cam)
{
	cameras.push_back(cam);
}

void Overlay::unlinkCamera(Camera* cam)
{
	auto index = std::find(cameras.begin(), cameras.end(), cam);
	if (index != cameras.end())
		cameras.erase(index);
}

void Overlay::switchCamera(Camera* cam)
{
	currentCamera = cam;
}

void Overlay::processInput(sbReal dTime)
{
	glfwPollEvents();
	int mouseState = input::getVal(input::MOUSE_STATE);
	int keyMods = input::getVal(input::KEY_MODS);

	if (glfwGetKey(wnd, GLFW_KEY_K))
	{
		if (keyMods & GLFW_MOD_CONTROL)
			g_tMul = 1.0;
		else if (keyMods & GLFW_MOD_ALT)
			g_tMul *= 0.97;
		else g_tMul *= 1.034;
	}

	if (glfwGetKey(wnd, GLFW_KEY_R)) {
		sb_object.setPosition(glm::dvec3(0.0, 135.0, 0.0));
		sb_object.setVelocity(glm::dvec3(0.0, 0.0, 0.0));
	}

	// Camera movement
	{
		int cmov = 0;

		if (glfwGetKey(wnd, GLFW_KEY_W))
			cmov = CAM_MOV_FW;
		else if (glfwGetKey(wnd, GLFW_KEY_S))
			cmov = CAM_MOV_BW;
		if (glfwGetKey(wnd, GLFW_KEY_A))
			cmov |= CAM_MOV_L;
		else if (glfwGetKey(wnd, GLFW_KEY_D))
			cmov |= CAM_MOV_R;
		if (glfwGetKey(wnd, GLFW_KEY_SPACE))
			cmov |= CAM_MOV_UP;
		else if (input::getVal(input::KEY_MODS) & GLFW_MOD_SHIFT)
			cmov |= CAM_MOV_DN;
		
		camera.applyVelDelta(cmov, dT);
		camera.updatePhysics(dT);
	}
	
	static bool objectGrabbed = false;
	if (mouseState & (1 << GLFW_MOUSE_BUTTON_RIGHT))
	{
		/* Grab or push the object */
		glm::dvec3 grab = camera.wPos + glm::dvec3(camera.forward * 8.f);
		glm::dvec3 objCoords = sb_object.getPosition();
		glm::dvec3 rope = grab - objCoords;

		if (!lcm::elenvec3(rope, 8.2))
		{
			if (mouseState & (1 << GLFW_MOUSE_BUTTON_LEFT))
			{
				// sb_object.setPosition(grab);
				// sb_object.setVelocity(camera.wVel);
				sb_object.setVelocity(camera.wVel, keyMods & GLFW_MOD_ALT);
			}
			else if (keyMods & GLFW_MOD_ALT)
			{
				sb_object.accelerate((glm::dvec3)camera.forward * (dT * 500.0));
				objectGrabbed = false;
			}
			else
			{
				if (!objectGrabbed)
					sb_object.setVelocity(camera.wVel);
				
				objectGrabbed = true;
				
				glm::dvec3 objvel = sb_object.getVelocity();
				// Indicates approx/moving away speed (if negative then approximating)
				double vcos = lcm::cosvec3(rope, objvel);
				glm::dvec3 acc = rope;
				glm::dvec3 diff = glm::dvec3(camera.wVel) - objvel;
				double diff_dot = glm::dot(diff, diff);
				if (diff_dot > 1.0)
					acc += 0.2 * diff / sqrt(diff_dot);
				acc *= 500.0;

				// Let the amplitude fade
				if ((vcos > 0.0))
					acc *= 0.5;
				// else acc *= 1.5;
				
				sb_object.accelerate(acc * dT);
				// sb_object.setPosition(grab);
				// sb_object.setVelocity(camera.wVel, false);
			}
		}
	}
	else
	{
		objectGrabbed = false;
		if (mouseState & (1 << GLFW_MOUSE_BUTTON_LEFT))
			sb_object.setVelocity(glm::dvec3(0.0), keyMods & GLFW_MOD_ALT);
	}

	sb_object.k_solid = g_hardness;
	sb_object.k_visc = g_visc;
	sb_object.k_frict = g_mu;

	double objdt = g_tMul * dT;

	objdt /= g_tPrec;
	for (long loop = 0; loop < g_tPrec; ++loop) {
		sb_object.updatePhysics(objdt);
	}

	graphics::g_markers[0].orig = graphics::g_markers[1].orig = sb_object.getPosition();
	graphics::g_markers[0].v = sb_object.getMassPoints()[g_markerIndex1].pos;
	graphics::g_markers[1].v = sb_object.getMassPoints()[g_markerIndex2].pos;
}

void Overlay::renderFrame()
{
	if (!(winWidth && winHeight))
		return;

	glm::mat4 projection;
	if (viewChanged)
		projection = glm::perspective(cam.FoV, (float)WinW / WinH, 0.1f, cam.renderDist);
	glm::mat4 view = cam.getViewMatrix();
	// glm::mat4 transform(1.f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, worldMap);
	
	// Display planet
	// {
	// 	planetShader.use();
	// 	if (viewChanged)
	// 		planetShader.setMat4("projection", projection);
	// 	planetShader.setMat4("view", view);
	// 	planetShader.setInt("worldmap", 0);
	// 	glBindVertexArray(plVAO);
	// 	glDrawElements(GL_TRIANGLES, sizeof(sphIndices) / sizeof(unsigned long), GL_UNSIGNED_INT, 0);
	// }
	
	// Display an object
	{
		stdShader.use();
		if (viewChanged)
			stdShader.setMat4("projection", projection);
		stdShader.setMat4("view", view);
		stdShader.setVec3("viewPos", (glm::vec3)cam.wPos);
		stdShader.setFloat("lightStrength", g_lightStrength);
		float currentTime = (float)glfwGetTime();
		stdShader.setFloat("glfwTime", currentTime);
		sb_obj.glRender(stdShader);
	}

	// Display custom sky (if available & enabled)
	// if (skyboxState == 3)
	// {
	// 	skyShader.use();

	// 	if (viewChanged)
	// 		skyShader.setMat4("projection", projection);
	// 	skyShader.setMat4("view", glm::mat4(glm::mat3(view))); // remove translation from the view matrix
	// 	skyShader.setInt("skyTex", 1);

	// 	glActiveTexture(GL_TEXTURE1);
	// 	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);

	// 	// draw skybox as last
	// 	glDepthFunc(GL_LEQUAL);
	// 	glBindVertexArray(skyboxVAO);
	// 	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// 	glDepthFunc(GL_LESS);
	// 	glBindVertexArray(0);
	// }
	
	// Display markers
	// mShader.use();
	// mShader.setMat4("view", view);
	// if (viewChange)
	// 	mShader.setMat4("projection", projection);

	// glBindVertexArray(mVAO);
	// glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	// for (auto& mrk: g_markers)
	// {
	// 	glBufferData(GL_ARRAY_BUFFER, sizeof(Marker), &mrk, GL_DYNAMIC_DRAW);
		
	// 	// Draw first in GL_LESS not to rewrite depth values of object faces
	// 	mShader.setVec4("fragColor", glm::vec4(0.f, 1.f, 0.f, 1.f));
	// 	glDrawArrays(GL_LINE_STRIP, 0, 2);
	// 	glDrawArrays(GL_LINE_STRIP, 2, 2);
		
	// 	// Draw white line only where green is covered
	// 	glDepthFunc(GL_NOTEQUAL);
	// 	mShader.setVec4("fragColor", glm::vec4(1.f, 1.f, 1.f, 1.f));
	// 	glDrawArrays(GL_LINE_STRIP, 0, 2);
	// 	glDrawArrays(GL_LINE_STRIP, 2, 2);
		
	// 	glDepthFunc(GL_LESS);
	// }
	glfwSwapBuffers(context);
}

static double num_sign(double val)
{
	if (val < 0.0)
		return -1.0;
	return 1.0;
}

int getVal(cbEnum query)
{
	switch(query)
	{
		case KEY_MODS: return kMods;
		case MOUSE_STATE: return mState;
		default: return 0;
	}
}

void keyCallbackEvent(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
	kMods = mods;
	if (action != GLFW_RELEASE)
	{
		switch(key)
		{
		case GLFW_KEY_ESCAPE:
			if (glfwGetInputMode(wnd, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
				glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			break;
		case GLFW_KEY_G:
			g_visc = step_clamp_linear(g_visc);
			break;
		case GLFW_KEY_U:
			g_forceMul = step_pow(g_forceMul);
			break;
		case GLFW_KEY_H:
			g_hardness = step_pow(g_hardness);
			break;
		case GLFW_KEY_M:
			g_plMass = step_pow(g_plMass);
			break;
		case GLFW_KEY_F:
			g_mu = step_clamp_linear(g_mu);
			break;
		case GLFW_KEY_B:
			std::cout << "--values:\n\tg_hardness == " << g_hardness <<
				"\n\tg_mu == " << g_mu <<
				"\n\tg_visc == " << g_visc <<
				"\n\tg_tPrec == " << g_tPrec <<
				"\n\tg_tMul == " << g_tMul <<
				"\n\tg_forceMul == " << g_forceMul <<
				"\n\tg_plMass == " << g_plMass <<
				"\n\tg_lightStrength == " << g_lightStrength <<
				"\n\tg_markerIndex1 == " << g_markerIndex1 <<
				"\n\tg_markerIndex2 == " << g_markerIndex2 <<
				"\n\tPosition: "; print(getObject()->getPosition());
			std::cout << "\tVelocity: "; print(getObject()->getVelocity());
			break;
		case GLFW_KEY_Y:
			if (action == GLFW_PRESS)
				graphics::toggleSkybox();
			break;
		case GLFW_KEY_C:
			g_markerIndex1 = step_marker(g_markerIndex1);
			break;
		case GLFW_KEY_V:
			g_markerIndex2 = step_marker(g_markerIndex2);
			break;
		case GLFW_KEY_N:
			g_forceMul = -g_forceMul;
			break;
		}
	}
}

void framebufResizeCallbackEvent(GLFWwindow* wnd, int width, int height)
{
	graphics::resizeCallbackEvent(wnd, width, height);
}

void mouseButtonCallbackEvent(GLFWwindow* wnd, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action != GLFW_RELEASE)
		{
			input::mState |= (1 << GLFW_MOUSE_BUTTON_LEFT);
			if (glfwGetInputMode(wnd, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
			{
				glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				getCurrentCamera()->fInput |= CAM_MOUSE_ENTRY;
			}
		}
		else input::mState &= ~(1 << GLFW_MOUSE_BUTTON_LEFT);
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action != GLFW_RELEASE)
			input::mState |= (1 << GLFW_MOUSE_BUTTON_RIGHT);
		else input::mState &= ~(1 << GLFW_MOUSE_BUTTON_RIGHT);
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		if (action != GLFW_RELEASE)
		{
			input::mState |= (1 << GLFW_MOUSE_BUTTON_MIDDLE);
			currentCamera->renderDist = 500.f;
			currentCamera->fInput |= CAM_VIEW_CHANGE;
		}
		else input::mState &= ~(1 << GLFW_MOUSE_BUTTON_MIDDLE);
	}
}

void cursorPosCallbackEvent(GLFWwindow* wnd, double xpos, double ypos)
{
	if (glfwGetInputMode(wnd, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		currentCamera->applyRotation((float)xpos, (float)ypos);
}

void scrollCallbackEvent(GLFWwindow* wnd, double, double yoffset)
{
	if (kMods & GLFW_MOD_CONTROL)
	{
	}
	else getCurrentCamera()->glfwScrollCallback((float)yoffset, kMods);
}

static unsigned long step_marker(unsigned long index)
{
	if (kMods & GLFW_MOD_ALT) --index;
	else ++index;

	index %= getObject()->getNumMassPoints();
	return index;
}

static double step_clamp_linear(double val)
{
	if (kMods & GLFW_MOD_ALT)
		val -= 0.05;
	else val += 0.05;

	if		(val < 0.0) val = 0.0;
	else if (val > 1.0) val = 1.0;
	return val;
}

static double step_pow(double val)
{
	if (kMods & GLFW_MOD_ALT)
		val /= 1.1;
	else val *= 1.1;
	return val;
}

NAMESPACE_END(cm3d)