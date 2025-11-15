#include "Includes.h"

bool SteamOverlaySetup::Setup()
{
	// Initialize MinHook
	if (MH_Initialize() != MH_OK)
		return false;

	// Create a temporary D3D11 device and swap chain to get the vtable
	WNDCLASSEX windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = L"DX11TempWindow";
	windowClass.hIconSm = NULL;

	RegisterClassEx(&windowClass);

	HWND tempWindow = CreateWindow(
		windowClass.lpszClassName,
		L"DirectX11 Temporary Window",
		WS_OVERLAPPEDWINDOW,
		0, 0,
		100, 100,
		NULL, NULL,
		windowClass.hInstance,
		NULL
	);

	if (!tempWindow)
	{
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = 100;
	swapChainDesc.BufferDesc.Height = 100;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = tempWindow;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain,
		&device,
		NULL,
		&context
	);

	if (FAILED(result))
	{
		DestroyWindow(tempWindow);
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return false;
	}

	// Get the vtable from the swap chain
	void** swapChainVTable = *reinterpret_cast<void***>(swapChain);

	// Present is at index 8, ResizeBuffers is at index 13
	m_presentTarget = swapChainVTable[8];
	m_resizeBuffersTarget = swapChainVTable[13];

	// Cleanup
	if (swapChain) swapChain->Release();
	if (context) context->Release();
	if (device) device->Release();
	DestroyWindow(tempWindow);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);

	if (!m_presentTarget || !m_resizeBuffersTarget)
		return false;

	return true;
}

bool SteamOverlaySetup::Hook()
{
	// Hook Present using MinHook
	if (MH_CreateHook(m_presentTarget, &Hooks::PresentHook, reinterpret_cast<LPVOID*>(&Hooks::oPresent)) != MH_OK)
		return false;

	// Hook ResizeBuffers using MinHook
	if (MH_CreateHook(m_resizeBuffersTarget, &Hooks::ResizeBuffersHook, reinterpret_cast<LPVOID*>(&Hooks::oResizeBuffers)) != MH_OK)
		return false;

	// Enable the hooks
	if (MH_EnableHook(m_presentTarget) != MH_OK)
		return false;

	if (MH_EnableHook(m_resizeBuffersTarget) != MH_OK)
		return false;

	return true;
}