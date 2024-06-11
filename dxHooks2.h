#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include <dxgi1_2.h>

/**
 * @brief Function pointer type for CreateSwapChainForHwnd.
 *
 * This function is used to create a swap chain for a given HWND.
 *
 * @param This The pointer to the IDXGIFactory2 interface.
 * @param pDevice The pointer to the ID3D11Device interface.
 * @param hWnd The handle to the window.
 * @param pDesc The pointer to the DXGI_SWAP_CHAIN_DESC1 structure that describes the swap chain.
 * @param pFullscreenDesc The pointer to the DXGI_SWAP_CHAIN_FULLSCREEN_DESC structure that describes the fullscreen swap chain.
 * @param pRestrictToOutput The pointer to the IDXGIOutput interface that restricts the swap chain to a specific output.
 * @param ppSwapChain The pointer to the IDXGISwapChain1 interface that receives the created swap chain.
 * @return HRESULT Returns S_OK if successful, otherwise an error code.
 */
typedef HRESULT(STDMETHODCALLTYPE* CreateSwapChainForHwnd_type)(
    IDXGIFactory2* This,
    _In_  ID3D11Device* pDevice,
    _In_  HWND hWnd,
    _In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
    _In_opt_  const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
    _In_opt_  IDXGIOutput* pRestrictToOutput,
    _COM_Outptr_  IDXGISwapChain1** ppSwapChain);

void initDxHooks2();
extern void removeDXHooks();
extern void RemoveImGuiD3D11();
extern float pFOV;