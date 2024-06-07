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
	bool drawLine;
	bool drawNear;
	bool drawFar;
	float farDistance;
	float minPhysMass;
	bool statsMode;
	bool debugMode;
	ImColor farColor;
	ImColor lineFarColor;
	ImColor nearColor;
};

static std::vector<AsteroidSubData> asteroidsSubData;
static std::vector<AsteroidCache> asteroidsCache;

static float calculateDistance(const physx::PxVec3& pos1, const physx::PxVec3& pos2) {
	physx::PxVec3 diff = pos1 - pos2;
	return sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
}

static void drawAsteroid(const physx::PxVec3& plyPos, const physx::PxVec3& asteroidPos, const char* type, float farDist, const AsteroidRenderingSettings& settings, const ImGuiIO& io) {
	//todo: W2S doesnt seem to properly take into account FOV (such as when zooming)
	
	float dist = calculateDistance(plyPos, asteroidPos);
	std::string buff = std::string(type) + " " + std::to_string(static_cast<int>(dist));

	if (dist > settings.farDistance) {
		if (settings.drawFar) {
			physx::PxVec2 screenPos = worldToScreen(asteroidPos);

			//todo: this is a bad hardwire. we should use the player resolution to clamp rendering angles
			if (screenPos.x > 0 && screenPos.y > 0 && screenPos.x < 1950 && screenPos.y < 1100) {
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
}

static AsteroidRenderingSettings loadRenderingSettings() {
	AsteroidRenderingSettings settings;
	settings.drawLine = getOption<bool>("drawAsteroidLine");
	settings.drawFar = getOption<bool>("drawFarAsteroid");
	settings.drawNear = getOption<bool>("drawNearAsteroid");
	settings.farDistance = getOption<float>("farAsteroidDistance");
	settings.minPhysMass = getOption<float>("minPhysMass");
	settings.debugMode = getOption<bool>("debugMode");
	settings.farColor = getOption<ImColor>("farAsteroidColor");
	settings.nearColor = getOption<ImColor>("nearAstreoidColor");
	settings.lineFarColor = getOption<ImColor>("lineAsteroidColor");
	settings.statsMode = getOption<bool>("statsMode");

	return settings;
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

	uint32_t maxObjects = *(uint32_t*)(objectManager + 0x60068);

	ImGuiIO& io = ImGui::GetIO();
	bool checkOre = getOption<bool>("asteroidOreCheck");
	AsteroidRenderingSettings renderSettings = loadRenderingSettings();

	for (uint64_t i = 0; i < maxObjects; i++) {
		asteroidStruct* object = (asteroidStruct*)((*(uint64_t*)(objectManager + 0x60060) & 0xFFFFFFFFFFFFFFFCui64) + (0x150 * i));
		if (testObjectPtr(object)) {
			continue;
		}

		if (checkOre && !strstr(object->type, "ore")) {
			continue;
		}

		std::vector<const char*> asteroid = { object->type };
		parseAsteroids(asteroid);

		physx::PxVec3 objectPos{ object->x, object->y, object->z };
		float dist = calculateDistance(ply.pos, objectPos);

		if (asteroidsSubData.size() <= i) {
			asteroidsSubData.resize(uint64_t((i + 1) * 1.2));
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