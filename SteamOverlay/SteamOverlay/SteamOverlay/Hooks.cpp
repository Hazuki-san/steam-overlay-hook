#include "Includes.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	return CallWindowProcA(Hooks::ImGuiData::oWndProc, hWnd, msg, wParam, lParam);
}

HRESULT __stdcall Hooks::PresentHook(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)
{
	static bool isSetup = false;
	if (!isSetup)
	{
		if (SUCCEEDED(swapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Hooks::ImGuiData::d3d11Device)))
		{
			Hooks::ImGuiData::d3d11Device->GetImmediateContext(&Hooks::ImGuiData::deviceContext);

			DXGI_SWAP_CHAIN_DESC sd;
			swapChain->GetDesc(&sd);

			Hooks::ImGuiData::window = sd.OutputWindow;

			ID3D11Texture2D* backBuffer = nullptr;
			swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
			Hooks::ImGuiData::d3d11Device->CreateRenderTargetView(backBuffer, nullptr, &Hooks::ImGuiData::deviceRenderTargetView);
			backBuffer->Release();

			Hooks::ImGuiData::oWndProc = (WNDPROC)SetWindowLongPtr(Hooks::ImGuiData::window, GWLP_WNDPROC, (LONG_PTR)WndProc);

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
			io.IniFilename = NULL;
			ImGui_ImplWin32_Init(Hooks::ImGuiData::window);
			ImGui_ImplDX11_Init(Hooks::ImGuiData::d3d11Device, Hooks::ImGuiData::deviceContext);

			isSetup = true;
		}
		else
		{
			return oPresent(swapChain, syncInterval, flags);
		}
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Stylish System Clock
	{
		// Get current time
		SYSTEMTIME st;
		GetLocalTime(&st);

		// Create a stylish clock window
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
										ImGuiWindowFlags_AlwaysAutoResize |
										ImGuiWindowFlags_NoSavedSettings |
										ImGuiWindowFlags_NoFocusOnAppearing |
										ImGuiWindowFlags_NoNav;

		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos;
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;

		// Position in top-right corner
		window_pos.x = work_pos.x + work_size.x - PAD;
		window_pos.y = work_pos.y + PAD;
		window_pos_pivot.x = 1.0f;
		window_pos_pivot.y = 0.0f;

		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowBgAlpha(0.75f); // Slightly transparent background

		if (ImGui::Begin("SystemClock", nullptr, window_flags))
		{
			// Format time string
			char timeStr[32];
			sprintf_s(timeStr, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);

			// Format date string
			const char* weekdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
			const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
									 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
			char dateStr[64];
			sprintf_s(dateStr, "%s, %s %02d, %d",
					  weekdays[st.wDayOfWeek],
					  months[st.wMonth - 1],
					  st.wDay,
					  st.wYear);

			// Display with styling
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.8f, 1.0f, 1.0f)); // Cyan color for time
			ImGui::SetWindowFontScale(2.0f);
			ImGui::Text("%s", timeStr);
			ImGui::SetWindowFontScale(1.0f);
			ImGui::PopStyleColor();

			ImGui::Spacing();

			// Display date
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f)); // Light gray for date
			ImGui::Text("%s", dateStr);
			ImGui::PopStyleColor();
		}
		ImGui::End();
	}

	ImGui::Render();

	Hooks::ImGuiData::deviceContext->OMSetRenderTargets(1, &Hooks::ImGuiData::deviceRenderTargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return oPresent(swapChain, syncInterval, flags);
}

HRESULT __stdcall Hooks::ResizeBuffersHook(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
{
	if (Hooks::ImGuiData::deviceRenderTargetView)
	{
		Hooks::ImGuiData::deviceContext->OMSetRenderTargets(0, 0, 0);
		Hooks::ImGuiData::deviceRenderTargetView->Release();
	}

	HRESULT result = oResizeBuffers(swapChain, bufferCount, width, height, newFormat, swapChainFlags);

	ID3D11Texture2D* buffer = nullptr;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

	Hooks::ImGuiData::d3d11Device->CreateRenderTargetView(buffer, NULL, &Hooks::ImGuiData::deviceRenderTargetView);
	buffer->Release();

	Hooks::ImGuiData::deviceContext->OMSetRenderTargets(1, &Hooks::ImGuiData::deviceRenderTargetView, NULL);

	D3D11_VIEWPORT vp{};
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	Hooks::ImGuiData::deviceContext->RSSetViewports(1, &vp);
	return result;
}