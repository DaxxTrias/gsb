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

uintptr_t localEnt;
uintptr_t localEnt_VelocityVec3;

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
	
    uintptr_t initialOffset = 0xAF99568;
    uintptr_t nextOffset = 0xC4C;

    localEnt = *reinterpret_cast<uintptr_t*>(baseAddress + initialOffset);
    localEnt_VelocityVec3 = localEnt + 0xC4C;

	float localEnt_VelocityX = 0.0f;
	float localEnt_VelocityY = 0.0f;
	float localEnt_VelocityZ = 0.0f;

    if (localEnt != 0 && localEnt_VelocityVec3 != 0) {
        localEnt_VelocityX = *reinterpret_cast<float*>(localEnt_VelocityVec3);
        localEnt_VelocityY = *reinterpret_cast<float*>(localEnt_VelocityVec3 + 0x4);
        localEnt_VelocityZ = *reinterpret_cast<float*>(localEnt_VelocityVec3 + 0x8);
    }

    physx::PxVec3 localEnt_VelocityVector3 = createPxVec3(localEnt_VelocityX, localEnt_VelocityY, localEnt_VelocityZ);
    float speed = calculateVelocity(localEnt_VelocityVector3);

	uint32_t obj = maxObjects;
	char buffer[256];

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
	snprintf(buffer, sizeof(buffer), "Coords: [%.0f, %.0f, %.0f]", ply.pos.x, ply.pos.y, ply.pos.z);
	ImGui::GetWindowDrawList()->AddText(
		ImGui::GetFont(), ImGui::GetFontSize(),
		ImVec2(5, 40), settings.drawStatsColor, buffer);

	// Draw velocity
	snprintf(buffer, sizeof(buffer), "Velocity: [%.0f, %.0f, %.0f] %.0f m/s", localEnt_VelocityX, localEnt_VelocityY, localEnt_VelocityZ, speed);
	ImGui::GetWindowDrawList()->AddText(
		ImGui::GetFont(), ImGui::GetFontSize(),
		ImVec2(5, 60), settings.drawStatsColor, buffer);
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

	//maxObjects = *(uint32_t*)(objectManager + 0x60068);

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

		bool isAsteroid = parseAsteroids(asteroid);

		if (isAsteroid && !checkOre)
			continue;

		physx::PxVec3 objectPos{ object->x, object->y, object->z };
		float dist = calculateDistance(ply.pos, objectPos);

		if (asteroidsSubData.size() <= i) {
			asteroidsSubData.resize(uint64_t((i + 1) * 1.2));
		}

		if (dist < 5.0f && renderSettings.debugMode)
		{
			fprintf(Con::fpout, "ObjAdd: %p Dist: %f idx: %ild\n", object, dist, reinterpret_cast<uintptr_t>(object));
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