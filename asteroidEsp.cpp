#include "asteroidEsp.h"
#include "gameHooks.h"
#include "render.h"
#include <foundation/PxVec2.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "menu.h"
#include "asteroidFiltering.h"
#include <vector>
#include "console.h"
#include <PxPhysicsAPI.h>

struct AsteroidSubData {
	float maxDist;
	asteroidStruct* ptr;
	void* subPtr;
};

struct AsteroidCache {
	physx::PxVec3 pos;
	char type[256];
	float dist;
	uint32_t ind;
};

struct AsteroidRenderingSettings {
	bool drawLine = false;
	bool drawNear = true;
	bool drawFar = false;
	float farDistance = 2000.f;
	float minPhysMass = 300.f;
	bool drawStats = true;
	bool debugMode = false;
	bool crossHair = true;
	ImColor farColor;
	ImColor lineFarColor;
	ImColor drawStatsColor;
	ImColor nearColor;
};

static AsteroidRenderingSettings loadRenderingSettings() {
	AsteroidRenderingSettings settings;
	settings.drawLine = getOption<bool>("drawAsteroidLine");
	settings.drawFar = getOption<bool>("drawFarAsteroid");
	settings.drawNear = getOption<bool>("drawNearAsteroid");
	settings.farDistance = getOption<float>("farAsteroidDistance");
	settings.minPhysMass = getOption<float>("minPhysMass");
	settings.debugMode = getOption<bool>("debugMode");
	settings.crossHair = getOption<bool>("drawCrosshair");
	settings.farColor = getOption<ImColor>("farAsteroidColor");
	settings.nearColor = getOption<ImColor>("nearAstreoidColor");
	settings.lineFarColor = getOption<ImColor>("lineAsteroidColor");
	settings.drawStatsColor = getOption<ImColor>("drawStatsColor");
	settings.drawStats = getOption<bool>("drawStats");

	return settings;
}
using namespace physx;

static std::vector<AsteroidSubData> asteroidsSubData;
static std::vector<AsteroidCache> asteroidsCache;

bool atMainMenu = false;
__int8 currentControllers;
uintptr_t localPlayer;
uintptr_t PxControllerObject_Context;
uintptr_t localPlayer_VelocityVec3;
uintptr_t localPlayerPtr = 0xAF99568; // v922 (close by but not exact pattern) 48 89 05 ? ? ? ? 48 8D 15 ? ? ? ? 48 89 5C 24 ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 42 8B 04 37 48 8B F7 39 05 ? ? ? ? 0F 8E ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 44 39 3D ? ? ? ? 0F 85 ? ? ? ? B8 ? ? ? ? 48 8D 1D ? ? ? ? 66 89 05 ? ? ? ? 48 8D 0D ? ? ? ? 49 8B C7 48 FF C0
//uintptr_t localPlayerInitialOffsetSTU = 0x2FBE1E38; // v100042 also nearby is a bunch of other interesting things like sendChatMessage
//uintptr_t localPlayerPtrSTU = 0x2F8A0748; // v100043 they did a bunch of rewrites in this section. yay fun.
//uintptr_t localPlayerPtrSTU = 0x2F8A07A8; // v100044
//uintptr_t localPlayerPtrSTU = 0x2F7930D8; // v100046
//uintptr_t localPlayerPtrSTU = 0x2F7934D8; // v100047
//uintptr_t localPlayerPtrSTU = 0x2F794A98; // v100048
//uintptr_t localPlayerPtrSTU = 0x2F796818; // v100050
//uintptr_t localPlayerPtrSTU = 0x2F7968E8; // v100052
//uintptr_t localPlayerPtrSTU = 0x2F7978A8; // v100053
//uintptr_t localPlayerPtrSTU = 0x2F79A568; // v100055
//uintptr_t localPlayerPtrSTU = 0x2F8110D8; // v100056
//uintptr_t localPlayerPtrSTU = 0x2F8101C8; // v100057
//uintptr_t localPlayerPtrSTU = 0x2F81B918; // v100058
//uintptr_t localPlayerPtrSTU = 0x2F822CC8; // v100059
//uintptr_t localPlayerPtrSTU = 0x2F825EF8; // v100060
uintptr_t localPlayerPtrSTU = 0x2F83CCE8; // v100063
uintptr_t localPlayerVelocityOffsetSTU = 0xB70;
uintptr_t localPlayerVelocityOffset = 0xC4C;

static float calculateDistance(const physx::PxVec3& pos1, const physx::PxVec3& pos2) {
	physx::PxVec3 diff = pos1 - pos2;
	return sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
}

static float calculateVelocity(const physx::PxVec3& vel) {
	static physx::PxVec3 prevVel(0.0f, 0.0f, 0.0f);
	static float cachedVelocity = 0.0f;

	if (vel != prevVel) {
		cachedVelocity = sqrtf(vel.x * vel.x + vel.y * vel.y + vel.z * vel.z);
		prevVel = vel;
	}
	return cachedVelocity;
	//maybe just do return vec.magnitude(); and nothing else?
}

static physx::PxVec3 createPxVec3(float x, float y, float z) {
	return physx::PxVec3(x, y, z);
}

void drawCrosshair() {
	ImGuiIO& io = ImGui::GetIO();
	AsteroidRenderingSettings settings = loadRenderingSettings();

	char buffer[256];

	snprintf(buffer, sizeof(buffer), "*"); // ● <-- U+25CF
	ImGui::GetWindowDrawList()->AddText(
		ImGui::GetFont(), ImGui::GetFontSize() + 3,
		ImVec2(io.DisplaySize.x / 2 - 4, io.DisplaySize.y / 2 - 8), settings.drawStatsColor, buffer);
}

void drawStats(const bodyData& ply) {
	if (objectManager == 0) {
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	AsteroidRenderingSettings settings = loadRenderingSettings();

	// Check if PxControllerObject is null
	if (PxControllerObject == 0) {
		currentControllers = 0;
		atMainMenu = true;
		//return;
	}
	else
		atMainMenu = false;

	if (!atMainMenu)
	{
		try {
			// Check if PxControllerObject_Context is valid before dereferencing
			PxControllerObject_Context = *reinterpret_cast<uintptr_t*>(PxControllerObject + 0x208);

			// Additional check for PxControllerObject_Context before dereferencing
			if (PxControllerObject_Context != 0) {
				currentControllers = *reinterpret_cast<__int8*>(PxControllerObject_Context + 0x68);
			}
			else {
				currentControllers = 0;
			}
		}
		catch (const std::exception& e) {
			currentControllers = 0;
		}
		catch (...) {
			currentControllers = 0;
		}
	}

    localPlayer = *reinterpret_cast<uintptr_t*>(baseAddress + localPlayerPtr);
    localPlayer_VelocityVec3 = localPlayer + localPlayerVelocityOffset;

	float localEnt_VelocityX = 0.0f;
	float localEnt_VelocityY = 0.0f;
	float localEnt_VelocityZ = 0.0f;

    if (localPlayer != 0 && localPlayer_VelocityVec3 != 0) {
        localEnt_VelocityX = *reinterpret_cast<float*>(localPlayer_VelocityVec3);
        localEnt_VelocityY = *reinterpret_cast<float*>(localPlayer_VelocityVec3 + 0x4);
        localEnt_VelocityZ = *reinterpret_cast<float*>(localPlayer_VelocityVec3 + 0x8);
    }

    physx::PxVec3 localEnt_VelocityVector3 = createPxVec3(localEnt_VelocityX, localEnt_VelocityY, localEnt_VelocityZ);
    float speed = calculateVelocity(localEnt_VelocityVector3);

	uint32_t obj = maxObjects;
	char buffer[256];

	float posX = 0.f;
	float posY = 0.f;
	float posZ = 0.f;

	if (currentControllers > 0)
	{
		posX = ply.pos.x;
		posY = ply.pos.y;
		posZ = ply.pos.z;
	}

	// Draw "Stats" header
	snprintf(buffer, sizeof(buffer), "Stats");
	ImGui::GetWindowDrawList()->AddText(
		ImGui::GetFont(), ImGui::GetFontSize(),
		ImVec2(5,0), settings.drawStatsColor, buffer);

	// Draw number of entities/asteroids
	snprintf(buffer, sizeof(buffer), "Objects: %d", obj);
	ImGui::GetWindowDrawList()->AddText(
		ImGui::GetFont(), ImGui::GetFontSize(),
		ImVec2(5, 20), settings.drawStatsColor, buffer);

	// Draw coordinates
	snprintf(buffer, sizeof(buffer), "Coords: [%.0f, %.0f, %.0f]", posX, posY, posZ);
	ImGui::GetWindowDrawList()->AddText(
		ImGui::GetFont(), ImGui::GetFontSize(),
		ImVec2(5, 40), settings.drawStatsColor, buffer);

	// Draw velocity
	snprintf(buffer, sizeof(buffer), "Velocity: [%.0f, %.0f, %.0f] %.0f m/s", localEnt_VelocityX, localEnt_VelocityY, localEnt_VelocityZ, speed);
	ImGui::GetWindowDrawList()->AddText(
		ImGui::GetFont(), ImGui::GetFontSize(),
		ImVec2(5, 60), settings.drawStatsColor, buffer);

	// Draw current controllers (players/actors)
	snprintf(buffer, sizeof(buffer), "Controllers: %d", currentControllers);
	ImGui::GetWindowDrawList()->AddText(
		ImGui::GetFont(), ImGui::GetFontSize(),
		ImVec2(5, 80), settings.drawStatsColor, buffer);
}

static void drawAsteroid(const physx::PxVec3& plyPos, const physx::PxVec3& asteroidPos, const char* type, float farDist, const AsteroidRenderingSettings& settings, const ImGuiIO& io) {
	//todo: W2S doesnt seem to properly take into account FOV (such as when zooming)
	
	if (asteroidPos.x == 0 && asteroidPos.y == 0 && asteroidPos.z == 0)
		return;

	float dist = calculateDistance(plyPos, asteroidPos);

	if (dist < 1.0f) {
		return;
	}

	std::string buff;
	if (strchr(type, '\n') != nullptr) {
		buff = std::to_string(static_cast<int>(dist));
	}
	else {
		buff = std::string(type) + " " + std::to_string(static_cast<int>(dist));
	}

	if (dist > settings.farDistance) {
		if (settings.drawFar) {
			physx::PxVec2 screenPos = worldToScreen(asteroidPos);

			if (screenPos.x > 0 && screenPos.y > 0 && screenPos.x <= io.DisplaySize.x && screenPos.y <= io.DisplaySize.y) {
				ImGui::GetWindowDrawList()->AddText(
					ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(screenPos.x, screenPos.y), settings.farColor, buff.c_str());
			}
			if (settings.drawLine) {
				physx::PxVec2 linePos = worldToScreenIgnoreDirection(asteroidPos);
				ImGui::GetWindowDrawList()->AddLine(
					ImVec2(linePos.x, linePos.y), ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), settings.lineFarColor);
			}
		}
	}
	else if (dist < settings.farDistance && settings.drawNear) {
		//todo: scale font size the closer the distance is
		physx::PxVec2 screenPos = worldToScreen(asteroidPos);
		if (screenPos.x > 0 && screenPos.y > 0) {
			ImGui::GetWindowDrawList()->AddText(
				ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(screenPos.x, screenPos.y), settings.nearColor, buff.c_str());
		}
	}
	/*else if (settings.debugMode) {
		physx::PxVec2 screenPos = worldToScreen(asteroidPos);
		if (screenPos.x > 0 && screenPos.y > 0) {
			ImGui::GetWindowDrawList()->AddText(
				ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(screenPos.x, screenPos.y), settings.nearColor, buff.c_str());
		}
	}*/
	else
		return;
}

static void drawAsteroidsFromCache(const bodyData& ply) {
	ImGuiIO& io = ImGui::GetIO();
	AsteroidRenderingSettings settings = loadRenderingSettings();

	for (const auto& asteroid : asteroidsCache) {
		drawAsteroid(ply.pos, asteroid.pos, asteroid.type, asteroid.dist, settings, io);
	}
}

static bool testObjectPtr(asteroidStruct* object) {
	bool skip = false;
	__try {
		if ((uint64_t)object < 0x10'0000'0000 || (uint64_t)object > 0x1'0000'0000'0000 || object->type == nullptr) {
			skip = true;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		skip = true;
	}
	return skip;
}

static asteroidStruct* getObject(uintptr_t objectManager, uint64_t index) {
	uintptr_t objectPtr = (*(uintptr_t*)(objectManager + 0x60068) & 0xFFFFFFFFFFFFFFFCui64) + (0x150 * index);
	if (objectPtr == 0) {
		throw new std::exception("Object pointer is null");
	}
	return reinterpret_cast<asteroidStruct*>(objectPtr);
}

void drawAsteroidESP(const bodyData& ply) {
	//todo: when approaching a space station all results vanish temporarily?
	//	todo: above behavior seems to also occur randomly even when no nearby stations. perhaps invalid data poisons the cache?
	//	todo: maybe whats happening is that the 'zone cube' is changing and thats resetting the array?
	//	todo: easy to prove, just setup the stats XYZ
	//todo: drawLine works now but its contingent on far asteroids, instead of working for either near or far
	//todo: even when esp disabled we still parse all the asteroids. we could optimize for perf in this scenario.
	//todo: use player resolution to clamp rendering angles. we did good to prevent negative XY, now lets present stuff from the other side

	if (objectManager == 0) {
		return;
	}

	static uint32_t updateCounter = 0;
	updateCounter++;

	if (updateCounter < 30) {
		drawAsteroidsFromCache(ply);
		return;
	}
	updateCounter = 0;
	asteroidsCache.clear();

	ImGuiIO& io = ImGui::GetIO();
	bool checkOre = getOption<bool>("asteroidOreCheck");
	AsteroidRenderingSettings renderSettings = loadRenderingSettings();

	for (uint64_t i = 0; i < maxObjects; i++) {
		asteroidStruct* object = (asteroidStruct*)((*(uint64_t*)(objectManager + 0x60060) & 0xFFFFFFFFFFFFFFFCui64) + (0x150 * i));
		if (testObjectPtr(object)) {
			continue;
		}

		if (!renderSettings.drawNear && !renderSettings.drawFar)
			continue;

		if (checkOre && !strstr(object->type, "ore")) {
			continue;
		}

		if (!checkOre && strstr(object->type, "ore"))
			continue;

		std::vector<const char*> asteroid = { object->type };

		if (!parseAsteroids(asteroid) && checkOre)
			continue;

		physx::PxVec3 objectPos{ object->x, object->y, object->z };
		float dist = calculateDistance(ply.pos, objectPos);

		if (asteroidsSubData.size() <= i) {
			asteroidsSubData.resize(uint64_t((i + 1) * 1.2));
		}

		if (dist < 15.0f && dist > 1.2f && renderSettings.debugMode)
		{
			//todo: not sure why i wrote this? why reference the pointer two different ways? 
			// and not even output 2nd correctly?
			/*fprintf(
				Con::fpout, "ObjAdd: %p Dist: %f idx: %ild\n", 
					object, dist, reinterpret_cast<uintptr_t>(object));*/

			fprintf(Con::fpout, "ObjAdd: %p Dist: %f idx: %i\n", object, dist, i);
		}

		AsteroidSubData& subData = asteroidsSubData[i];
		if (dist > 600) {
			if (subData.ptr != object || subData.subPtr != (void*)object->ptr0) {
				subData.ptr = object;
				subData.maxDist = dist;
				subData.subPtr = (void*)object->ptr0;
			}
			else if (subData.maxDist < dist) {
				subData.maxDist = dist;
			}
		}

		float maxDist = (subData.ptr == object) ? subData.maxDist : 0;

		drawAsteroid(ply.pos, objectPos, object->type, maxDist, renderSettings, io);
		

		AsteroidCache cache;
		cache.ind = static_cast<uint32_t>(i);
		cache.pos = objectPos;
		cache.dist = maxDist;
		strcpy(cache.type, object->type);
		asteroidsCache.push_back(cache);
	}
}