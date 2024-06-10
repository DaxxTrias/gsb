#include "menu.h"
#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <vector>
#include <unordered_map>
#include <json.hpp>
#include <string>
#include <fstream>
#include <streambuf>
#include <variant>
#include "console.h"
#include <iostream>
#include <filesystem>
#include <unordered_set>
#include <string>
#include "asteroidFiltering.h"
#include "main.h"

using nlohmann::json;

static bool showmenu = false;
static std::unordered_map<std::string, SettingsMeta> options;

SettingsMap *getSettingsMap() {
	return &options;
}

void loadConfig(std::string config_file)
{
	std::ifstream file(config_file);
	std::string str((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	json config = json::parse(str);

	for (auto& d : config.items()) {
		if (d.value().is_boolean()) {
			bool* ptr = new bool{ d.value().get<bool>() };
			options[d.key()] = ptr;
		} else if (d.value().is_string()) {
			std::string* ptr = new std::string( d.value().get<std::string>() );
			options[d.key()] = ptr;
		} else if (d.value().is_number()) {
			float* ptr = new float{ d.value().get<float>() };
			options[d.key()] = ptr;
		} else if (d.value().is_object()) {
			if (d.value().contains("r")
			 || d.value().contains("g")
			 || d.value().contains("b")
			 || d.value().contains("a")) {
				ImColor* ptr = new ImColor();
				ptr->Value.x = d.value().at("r").get<float>();
				ptr->Value.y = d.value().at("g").get<float>();
				ptr->Value.z = d.value().at("b").get<float>();
				ptr->Value.w = d.value().at("a").get<float>();
				options[d.key()] = ptr;
			}
		}
	}

}

void saveConfig(std::string configName)
{
	json config;

	for (auto& d : options) {
		if (bool** pval = std::get_if<bool*>(&d.second)) {
			config[d.first] = **pval;
		} if (float** pval = std::get_if<float*>(&d.second)) {
			config[d.first] = **pval;
		} if (std::string** pval = std::get_if<std::string*>(&d.second)) {
			config[d.first] = **pval;
		} if (ImColor** pval = std::get_if<ImColor*>(&d.second)) {
			const ImColor& val = **pval;
			config[d.first] = {};
			config[d.first]["r"] = val.Value.x;
			config[d.first]["g"] = val.Value.y;
			config[d.first]["b"] = val.Value.z;
			config[d.first]["a"] = val.Value.w;
		}
	}

	std::ofstream config_file;
	config_file.open(configName);
	config_file << config.dump();
	config_file.close();
};

template<typename T>
T *getOptionPtr(std::string name) {
	auto find = options.find(name);
	if (find == options.end()) {
		T* ptr = new T();
		options[name] = ptr;
		return ptr;
	}

	if (T** pval = std::get_if<T*>(&(find->second))) {
		return *pval;
	}

	return nullptr;
}

template<typename T>
bool getOptionSafe(std::string name, T &data) {
	auto find = options.find(name);
	if (find == options.end()) {
		return false;
	}

	if (T** pval = std::get_if<T*>(&(find->second))) {
		data = **pval;
		return true;
	}

	return false;
}

inline void ImGuiSettingEditColor4(const char *name) {
	const ImGuiColorEditFlags colorEditFlags = 
		ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoLabel;
	ImGui::ColorEdit4(name, (float*)&getOptionPtr<ImColor>(name)->Value, colorEditFlags);
}

inline void ImGuiSettingCheckBox(const char* name) {
	ImGui::Checkbox(name, getOptionPtr<bool>(name));
}
//SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, float power)
inline void ImGuiSettingSlider(const char* name) {
	ImGui::SliderFloat(name, getOptionPtr<float>(name),300.0f,9999.0f);
}

void drawMenu() {
	const ImGuiColorEditFlags colorEditFlags = 
		ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoLabel;

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		showmenu = !showmenu;
	}
	if (GetAsyncKeyState(VK_END) & 1) {
		//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Shutdown, g_hModule, 0, NULL);
	}
	
	//todo: add a mass filter to display. Or some other way to filter out the non-chunky rocks if desired.
	//todo: add a waypoint system. (Similar to navgrid. and always show these positions on the W2S regardless of distance if enabled
	if (showmenu) {
		ImGui::Begin("Menu");

		ImGui::Checkbox("asteroidEspEnabled", getOptionPtr<bool>("asteroidEspEnabled"));

		if (ImGui::TreeNode("Asteroid Filters")) {
			for (auto& [filter, isActive] : filterState.filters) {
				if (ImGui::Checkbox(filter.c_str(), &isActive)) {
					cacheValid = false;  // Invalidate cache when any filter changes
				}
			}
			ImGui::TreePop();
		}

		ImGuiSettingCheckBox("asteroidOreCheck");

		ImGuiSettingEditColor4("nearAstreoidColor");
		ImGui::SameLine();
		ImGuiSettingCheckBox("drawNearAsteroid");

		ImGuiSettingEditColor4("farAsteroidColor");
		ImGui::SameLine();
		ImGuiSettingCheckBox("drawFarAsteroid");
		ImGuiSettingSlider("farAsteroidDistance");

		ImGuiSettingEditColor4("lineAsteroidColor");
		ImGui::SameLine();
		ImGuiSettingCheckBox("drawAsteroidLine");
		
		ImGuiSettingCheckBox("drawPhysMass");
		ImGuiSettingSlider("minPhysMass");

		ImGuiSettingEditColor4("drawStatsColor");
		ImGui::SameLine();
		ImGuiSettingCheckBox("drawStats");

		ImGuiSettingCheckBox("drawCrosshair");

		ImGuiSettingCheckBox("debugMode");

		//todo: add a crosshair dot

		if (ImGui::Button("save config")) {
			saveConfig("gsb.cfg");
		}

		ImGui::End();
	}
}