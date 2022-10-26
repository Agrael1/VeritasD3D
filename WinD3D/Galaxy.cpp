#include "Galaxy.h"

void Galaxy::DoFrame(float dt)
{
	if (!wnd.IsActive())return;

	const auto s = dt * speed;
	wnd.Gfx().BeginFrame(0.2f, 0.2f, 0.2f);
	wnd.Gfx().SetCamera(cam.GetViewMatrix());
	if (wnd.DrawGrid())grid.Submit();

	ds.Submit();
	rg->Execute(wnd.Gfx());

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiDockNodeFlags_NoDockingInCentralNode);

	if (ImGui::Begin("Simulation speed"))
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();

	ProcessInput(dt);
	ds.Advance(dt);
	cam.SpawnControlWindow();

	// Present
	wnd.Gfx().EndFrame();
	rg->Reset();
}
