#include "dxShadersDump.h"
#include "fnv1a.h"
#include <d3d11.h>
#include <D3Dcompiler.h>

#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL; }

static WCHAR d3d11_shaders[MAX_PATH] = L"shaders";
static void ShaderDump(const char* type, const char* ext, uint64_t id, const void* data, DWORD size) {
	CreateDirectoryW(d3d11_shaders, NULL);

	WCHAR path[1024];
	wsprintfW(path, L"%s\\%S_%016I64x.%S", d3d11_shaders, type, id, ext);

	HANDLE f = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f != INVALID_HANDLE_VALUE) {
		DWORD written;
		BOOL ok = WriteFile(f, data, size, &written, NULL);
		if (!ok || written != size) {
			MessageBoxA(nullptr, "dumpShader", "Error writing to file!", MB_OK);
		}
		CloseHandle(f);
	} else {
		MessageBoxA(nullptr, "dumpShader", "Error creating output file!", MB_OK);
	}
}

static void ShaderDisassemble(const char* type, uint64_t id, const void* bytecode, DWORD bytecode_size) {
	UINT flags = D3D_DISASM_ENABLE_DEFAULT_VALUE_PRINTS | D3D_DISASM_ENABLE_INSTRUCTION_OFFSET;

	ID3DBlob* blob;
	HRESULT hr = D3DDisassemble(bytecode, bytecode_size, flags, "", &blob);
	if (SUCCEEDED(hr)) {
		LPVOID buffer = (DWORD*)blob->GetBufferPointer();
		SIZE_T buffer_size = blob->GetBufferSize();
		ShaderDump(type, "asm", id, buffer, (DWORD)buffer_size);
		SAFE_RELEASE(blob);
	}
}

void ShaderHook(ID3D11Device* device, const char* type, const void** bytecode, SIZE_T* bytecode_size) {
	uint64_t id = fnv1a(*bytecode, *bytecode_size);

	ShaderDump(type, "bin", id, *bytecode, (DWORD)*bytecode_size);
	ShaderDisassemble(type, id, *bytecode, (DWORD)*bytecode_size);

	WCHAR hlsl[1024];
	wsprintfW(hlsl, L"%s\\%S_%016I64x.hlsl", d3d11_shaders, type, id);
	if (GetFileAttributesW(hlsl) != INVALID_FILE_ATTRIBUTES) {
		D3D_FEATURE_LEVEL level = device->GetFeatureLevel();

		int version;
		if (level == D3D_FEATURE_LEVEL_11_1 || level == D3D_FEATURE_LEVEL_11_0) {
			version = 50;
		} else if (level == D3D_FEATURE_LEVEL_10_1) {
			version = 41;
		} else if (level == D3D_FEATURE_LEVEL_10_0) {
			version = 40;
		} else {
			MessageBoxA(nullptr, "errorOnShaderVersion", "errorOnShaderVersion", MB_OK);
			return;
		}

		char target[16];
		wsprintfA(target, "%s_%u_%u", type, version / 10, version % 10);

		ID3DBlob* code = NULL;
		ID3DBlob* error = NULL;

		HRESULT hr = D3DCompileFromFile(hlsl, NULL, NULL, "main", target, D3DCOMPILE_OPTIMIZATION_LEVEL2 | D3DCOMPILE_WARNINGS_ARE_ERRORS, 0, &code, &error);

		if (error != NULL) {
			WCHAR txt[1024];
			wsprintfW(txt, L"%s\\%S_%016I64x.hlsl.txt", d3d11_shaders, type, id);

			const void* error_data = (DWORD*)error->GetBufferPointer();
            DWORD error_size = static_cast<DWORD>(error->GetBufferSize());

			HANDLE f = CreateFileW(txt, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (f != INVALID_HANDLE_VALUE){
				DWORD written;
				BOOL ok = WriteFile(f, error_data, error_size, &written, NULL);
				if (!ok || written != error_size) {
					MessageBoxA(nullptr, "errorOnShaderFile", "errorOnShaderFile", MB_OK);
				}
				CloseHandle(f);
			}

			SAFE_RELEASE(error);
		}

		if (SUCCEEDED(hr) && code != NULL) {
			*bytecode = (DWORD*)code->GetBufferPointer();
			*bytecode_size = code->GetBufferSize();
		}
	}
}