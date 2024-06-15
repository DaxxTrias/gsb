#include "dxHooks2.h"
#include <Windows.h>
#include "hookUtils.h"
#include "MinHook.h"
#include "dxShadersHook.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include "render.h"
#include "menu.h"
#include "dxUtils.h"
#include "physUtils.h"
#include "asteroidEsp.h"
#include "physicEsp.h"
#include "KillSwitch.h"
#include <iostream>
#include "gameHooks.h"

typedef HRESULT(__stdcall* D3D11Present1Hook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags, 
	const DXGI_PRESENT_PARAMETERS* pPresentParameters);
typedef void(__stdcall* D3D11DrawHook) (ID3D11DeviceContext* pContext, UINT VertexCount, UINT StartVertexLocation);

static ID3D11Device* pDevice = NULL;
static ID3D11DeviceContext* pContext = NULL;
static CreateSwapChainForHwnd_type CreateSwapChainForHwnd_or;
static ID3D11RenderTargetView* mainRenderTargetViewD3D11;
static D3D11Present1Hook phookD3D11Present1 = NULL;
static D3D11DrawHook phookD3D11Draw = NULL;
static ID3D11DepthStencilState* DepthStencilState_TRUE = NULL; //depth off
static ID3D11DepthStencilState* DepthStencilState_FALSE = NULL; //depth off
static ID3D11DepthStencilState* DepthStencilState_ORIG = NULL; //depth on

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static HWND window = NULL;
WNDPROC oWndProc;

static DWORD_PTR* pSwapChainVtable = NULL;
static DWORD_PTR* pContextVTable = NULL;
static DWORD_PTR* pDeviceVTable = NULL;

static bool initonce = false;

uintptr_t renderingModule;
uintptr_t playerFOV;
float pFOV = 90;

static void InitImGuiD3D11()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

void RemoveImGuiD3D11()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

static LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return true;
	}
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}


HRESULT __stdcall hookD3D11Present1(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags, 
	const DXGI_PRESENT_PARAMETERS* pPresentParameters) {
	if (!initonce)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetViewD3D11);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGuiD3D11();

			// Create depthstencil state
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			depthStencilDesc.StencilEnable = FALSE;
			depthStencilDesc.StencilReadMask = 0xFF;
			depthStencilDesc.StencilWriteMask = 0xFF;
			// Stencil operations if pixel is front-facing
			depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			// Stencil operations if pixel is back-facing
			depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			pDevice->CreateDepthStencilState(&depthStencilDesc, &DepthStencilState_FALSE);

			GenerateTexture(pDevice, 0xff00ff00, DXGI_FORMAT_R10G10B10A2_UNORM); //DXGI_FORMAT_R32G32B32A32_FLOAT); //DXGI_FORMAT_R8G8B8A8_UNORM; 

			initonce = true;
		} else {
			return phookD3D11Present1(pSwapChain, SyncInterval, Flags, pPresentParameters);
		}
	}

	if (mainRenderTargetViewD3D11 == NULL)
	{
		ID3D11Texture2D* pBackBuffer = NULL;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetViewD3D11);
		pBackBuffer->Release();
	}
	
	ImGuiIO io = ImGui::GetIO();
	setDisplaySize(io.DisplaySize.x, io.DisplaySize.y);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	drawMenu();

	if (initonce) {
		
		if (GetAsyncKeyState(VK_F4) & 1) {
			killSwitch = !killSwitch;
			std::cout << "Killswitch: " << (killSwitch.load() ? "ON" : "OFF") << std::endl;
		}	

		//uintptr_t initialOffset = 0xA6296E8;
		//renderingModule = *reinterpret_cast<uintptr_t*>(baseAddress + initialOffset);
		//playerFOV = renderingModule + 0x21C;
		//pFOV = reinterpret_cast<float*>(playerFOV)[0];
		
		ImGui::Begin("Transparent", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

		//todo: can probably check if localEnt initialized, and if not assume on main menu and sleep the render/processing loop?
		//todo: buying a ship in the creator might be able to 'break' the data loc's we hook. playerCoords seem to get toasted for sure.
		//reloading to main menu fixes the core esp, but physics esp still broken even after relog from above event/issue

		if (!killSwitch.load())
		{
			std::vector<bodyData> bodys = {};
			bodys = generateBodyData();
			bodyData ply = getPlyByMass(bodys);
			setCamPos(ply.pos);
			if (getOption<bool>("asteroidEspEnabled"))
				drawAsteroidESP(ply);
			if (getOption<bool>("drawPhysMass"))
				drawPhysicsESP(bodys, ply);
			if (getOption<bool>("drawStats"))
				drawStats(ply);
			if (getOption<bool>("drawCrosshair"))
				drawCrosshair();
		}

		ImGui::End();
	}

	ImGui::Render();
	pContext->OMSetRenderTargets(1, &mainRenderTargetViewD3D11, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return phookD3D11Present1(pSwapChain, SyncInterval, Flags, pPresentParameters);
}

void __stdcall hookD3D11Draw(ID3D11DeviceContext* pContext, UINT VertexCount, UINT StartVertexLocation) {
	ID3D11Buffer* veBuffer;
	UINT veWidth = 0;
	UINT Stride;
	UINT veBufferOffset;
	D3D11_BUFFER_DESC veDesc;

	//get models
	pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);
	if (veBuffer) {
		veBuffer->GetDesc(&veDesc);
		veWidth = veDesc.ByteWidth;
	}
	if (NULL != veBuffer) {
		veBuffer->Release();
		veBuffer = NULL;
	}

	ID3D11Buffer* pscBuffer;
	UINT pscWidth = -1;
	D3D11_BUFFER_DESC pscdesc;

	//get pscdesc.ByteWidth
	pContext->PSGetConstantBuffers(0, 1, &pscBuffer);
	if (pscBuffer) {
		pscBuffer->GetDesc(&pscdesc);
		pscWidth = pscdesc.ByteWidth;
	}
	if (NULL != pscBuffer) {
		pscBuffer->Release();
		pscBuffer = NULL;
	}

	if (initonce && veWidth / 100 == 24 && !killSwitch.load()) {
		ID3D11Buffer* pWorldViewCB;
		pContext->VSGetConstantBuffers(0, 1, &pWorldViewCB);
		ID3D11Buffer* m_pCurWorldViewCB = NULL;
		m_pCurWorldViewCB = CopyBufferToCpuA(pContext, pDevice, pWorldViewCB);

		float* worldviewPtr;
		MapBuffer(pContext, m_pCurWorldViewCB, (void**)&worldviewPtr, NULL);
		memcpy(getStaticGameWorldView(), &worldviewPtr[0], sizeof(float[4][4]));
		UnmapBuffer(pContext, m_pCurWorldViewCB);

		updateWorldViewProj();
	}

	return phookD3D11Draw(pContext, VertexCount, StartVertexLocation);
}

static HRESULT STDMETHODCALLTYPE CreateSwapChainForHwnd_hook(IDXGIFactory2* This, _In_  ID3D11Device* pDevice, 
	_In_  HWND hWnd, _In_ const DXGI_SWAP_CHAIN_DESC1* pDesc, _In_opt_  const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, 
	_In_opt_  IDXGIOutput* pRestrictToOutput, _COM_Outptr_  IDXGISwapChain1** ppSwapChain) {

	//call the original func
	HRESULT result = FnCast("CreateSwapChainForHwnd", CreateSwapChainForHwnd_or)(
		This, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);

	// Check if successful
	if (FAILED(result) || !ppSwapChain || !*ppSwapChain) {
		return result;
	}

	// extract vtables
	pSwapChainVtable = (DWORD_PTR*)dynamic_cast<IDXGISwapChain*>(*ppSwapChain);
	if (!pSwapChainVtable) {
		return E_FAIL;
	}
	pSwapChainVtable = (DWORD_PTR*)pSwapChainVtable[0];

	ID3D11DeviceContext* pContext = nullptr;
	pDevice->GetImmediateContext(&pContext);
	if (!pContext) {
		return E_FAIL;
	}
	pContextVTable = (DWORD_PTR*)pContext;
	pContextVTable = (DWORD_PTR*)pContextVTable[0];

	pDeviceVTable = (DWORD_PTR*)pDevice;
	pDeviceVTable = (DWORD_PTR*)pDeviceVTable[0];
	//pDevice->shader

	// hook present
	if (MH_CreateHook((DWORD_PTR*)pSwapChainVtable[22], hookD3D11Present1, reinterpret_cast<void**>(&phookD3D11Present1)) != MH_OK) { 
			MessageBoxA(nullptr, "Failed to hook D3D11_1 Present", "Error", MB_OK); 
		return E_FAIL;
	}
	if (MH_EnableHook((DWORD_PTR*)pSwapChainVtable[22]) != MH_OK) { 
			MessageBoxA(nullptr, "Failed to enable D3D11_1 Present Hook", "Error", MB_OK); 
		return E_FAIL;
	}

	// hook draw
	if (MH_CreateHook((DWORD_PTR*)pContextVTable[13], hookD3D11Draw, reinterpret_cast<void**>(&phookD3D11Draw)) != MH_OK) { 
		MessageBoxA(nullptr, "Failed to hook D3D11_1 Draw", "Error", MB_OK); 
		return E_FAIL;
	}
	if (MH_EnableHook((DWORD_PTR*)pContextVTable[13]) != MH_OK) { 
		MessageBoxA(nullptr, "Failed to enable D3D11_1 Draw Hook", "Error", MB_OK); 
		return E_FAIL;
	}

	// init extra dx hooks
	initDxSharesHooks(pDeviceVTable);
	
	// adjust mem protection for the hooked func
	DWORD dwOld;
	if (!VirtualProtect(phookD3D11Present1, 2, PAGE_EXECUTE_READWRITE, &dwOld)) {
		MessageBoxA(nullptr, "Failed to change mem protection on present hook", "Error", MB_OK);
		return E_FAIL;
	}

	//flush output for debug
	fflush(Con::fpout);

	return result;

}

void initDxHooks2() {
	HMODULE gameOverlayRenderer = GetModuleHandleA("GameOverlayRenderer64.dll"); // CreateSwapChainForHwnd +0x8CEC0 (jun-4-2024) 
	if (!gameOverlayRenderer) {
		MessageBoxA(nullptr, "GameOverlayRenderer64.dll not found", "Error", MB_OK);
		return;
	}
	//todo: if we investigate steam overlay i bet we can find the area where it hooks the swapchain if it beats us to it
	// maybe we could just grab it from there? which would let us defeat the need to inject super early
	const char* CreateSwapChainForHwnd_pattern =
		"48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC 40 48 8B F1 49 8B D9 48 8D 0D ? ? ? ? 49 8B F8 4C 8B F2 E8 ? ? ? ? 48 8B 44 24";
	CreateSwapChainForHwnd_or = findSignature<CreateSwapChainForHwnd_type>((unsigned char*)gameOverlayRenderer, CreateSwapChainForHwnd_pattern);

	if (!CreateSwapChainForHwnd_or) {
		MessageBoxA(nullptr, "CreateSwapChainforHwnd not found", "Error", MB_OK);
		return;
	}

	placeHook("CreateSwapChainForHwnd", CreateSwapChainForHwnd_or, CreateSwapChainForHwnd_hook);
}

void removeDXHooks() {
	if (phookD3D11Present1) {
		MH_DisableHook((DWORD_PTR*)pSwapChainVtable[22]);
		MH_RemoveHook((DWORD_PTR*)pSwapChainVtable[22]);
	}
	if (phookD3D11Draw) {
		MH_DisableHook((DWORD_PTR*)pContextVTable[13]);
		MH_RemoveHook((DWORD_PTR*)pContextVTable[13]);
	}
	removeHook("CreateSwapChainForHwnd");
}