#include "physicEsp.h"
#include <DirectXMath.h>
#include "render.h"
#include <foundation/PxVec2.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "menu.h"

void drawPhysicsESP(std::vector<bodyData>& bodys, bodyData ply) {
	char buff[256];

	if (!getOption<bool>("drawPhysMass")) {
		return;
	}

	for (bodyData body : bodys) {

		if (body.mass < getOption<bool>("minPhysMass")) {
			continue;
		}
		sprintf(buff, "%f", body.mass);
		
		physx::PxVec2 screenPos = worldToScreen(body.pos);

		if (screenPos.x > 0 && screenPos.y > 0) {
			ImGui::GetWindowDrawList()->AddText(
				ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(screenPos.x, screenPos.y + 15), ImColor(0, 255, 255, 255), buff, 0, 0.0f, 0);
		}
	}
}
