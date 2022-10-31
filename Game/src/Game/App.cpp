#include <Game/App.h>
#include <imgui.h>
#include <Engine/Util/Utility.h>

constexpr COMDLG_FILTERSPEC filterSpecs[] =
{
	{L"Wavefront object file",L"*.obj"},
	{L"Collada", L"*.dae, *.xml"},
	{L"Blender", L"*.blend"},
	{L"Biovision BVH", L"*.bvh"},
	{L"3D Studio Max 3DS", L"*.3ds"},
	{L"3D Studio Max ASE", L"*.ase"},
	{L"Stanford Polygon Library", L"*.ply"},
	{L"AutoCAD DXF", L"*.dxf"},
	{L"IFC - STEP", L"*.ifc"},
	{L"Neutral File Format", L"*.nff"},
	{L"Sense8 WorldToolkit", L"*.nff"},
	{L"Valve Model", L"*.smd, *.vta"},
	{L"Quake I", L"*.mdl"},
	{L"Quake II", L"*.md2"},
	{L"Quake III", L"*.md3"},
	{L"Quake 3 BSP", L"*.pk3"},
	{L"RtCW", L"*.mdc"},
	{L"Doom 3 ", L"*.md5mesh, *.md5anim, *.md5camera"},
	{L"DirectX X", L"*.x"},
	{L"Quick3D", L"*.q3o, *.q3s"},
	{L"Raw Triangles", L"*.raw"},
	{L"AC3D", L"*.ac"},
	{L"Stereolithography", L"*.stl"},
	{L"Autodesk DXF", L"*.dxf"},
	{L"Irrlicht Mesh", L"*.irrmesh, *.xml"},
	{L"Irrlicht Scene", L"*.irr, *.xml"},
	{L"Object File Format", L"*.off"},
	{L"Terragen Terrain", L"*.ter"},
	{L"3D GameStudio Model", L"*.mdl"},
	{L"3D GameStudio Terrain", L"*.hmp"},
	{L"Ogre", L"*.mesh*.xml, *.skeleton*.xml, *.material"},
	{L"Milkshape 3D", L"*.ms3d"},
	{L"LightWave Model", L"*.lwo"},
	{L"LightWave Scene", L"*.lws"},
	{L"Modo Model", L"*.lxo"},
	{L"CharacterStudio Motion", L"*.csm"},
	{L"Stanford Ply", L"*.ply"},
	{L"TrueSpace", L"*.cob, *.scn"},
	{L"All Files", L"*.*"}
};

namespace dx = DirectX;

App::App(uint32_t width, uint32_t height)
	: wnd(width, height, "VTest")/*, light(wnd.Gfx()), grid(wnd.Gfx())*/, text(wnd.Gfx(), 1.0)/*, ss(wnd.Gfx(), 1.0)*/
	, model(new Model(wnd.Gfx(), R"(D:\Repos\WinD3D\Game\models\Wooden Crate\Wooden Crate.obj)"))
{
	opener.SetFileTypes(filterSpecs);
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(height) / float(width), 0.5f, 100.0f));
}
App::~App()
{
}

winrt::IAsyncAction App::InitializeAsync()
{
	co_await winrt::when_all(lights.InitializeAsync(wnd.Gfx()),
		sphere.InitializeAsync(lights, wnd.Gfx()));
	//co_await ss.InitializeAsync(wnd.Gfx(), 1.0);
	CreateRenderGraph();
	//ss.SetPos({ 10.0f,9.0f,2.5f });
	co_return;
}

int App::Go()
{
	float dt = 0.01f;
	while (true)
	{
		const auto a = wnd.ProcessMessages();
		if (a)
		{
			return (int)a.value();
		}
		DoFrame(dt);
	}
}

void App::DoFrame(float dt)
{
	if (!wnd.IsActive())return;

	const auto s = dt * speed;
	wnd.Gfx().BeginFrame(0.2f, 0.2f, 0.2f);
	wnd.Gfx().SetCamera(cam.GetViewMatrix());
	sphere.Bind(wnd.Gfx());
	lights.Bind(wnd.Gfx());
	//light.Bind(wnd.Gfx(), cam.GetViewMatrix());

	if (model)model->Submit();
	//light.Submit();
	//text.Submit();
	//ss.Submit();


	sphere.Submit();
	//if (wnd.DrawGrid())grid.Submit();
	rg->Execute(wnd.Gfx());


	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiDockNodeFlags_NoDockingInCentralNode);

	if (ImGui::Begin("Simulation speed"))
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();

	if (model)
		modelProbe.SpawnWindow(*model);	// imgui windows

	ProcessInput(dt);
	cam.SpawnControlWindow();
	sphere.SpawnControlWindow();
	//light.SpawnControlWindow();

	// Present
	wnd.Gfx().EndFrame();
	rg->Reset();
}

void App::ProcessInput(float dt)
{
	while (const auto e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
		{
		case 'F':
		case VK_INSERT:
			if (wnd.CursorEnabled())
			{
				bFlightMode = true;
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				bFlightMode = false;
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_ESCAPE:
			if (bFlightMode)
			{
				bFlightMode = false;
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
				break;
			}
			PostQuitMessage(0);
			return;
		}
	}

	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed(VK_SHIFT))
		{
			dt *= 2;
		}
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cam.Translate({ 0.0f,0.0f,dt });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate({ -dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cam.Translate({ dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed(VK_SPACE))
		{
			cam.Translate({ 0.0f,dt,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('C'))
		{
			cam.Translate({ 0.0f,-dt,0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cam.Rotate((float)delta->x, (float)delta->y);
		}
	}

	if (wnd.LoadCalled())
	{
		switch (state)
		{
			using enum ModelLoadState;
		case Unloaded:
			state = InProgress;
			ReloadModelAsync();
			break;
		case Finish:
		{
			model.reset(swap.release());
			ver::Codex::Trim();
			if (model)
				model->LinkTechniques(*rg);
			modelProbe.Reset();
			state = Unloaded;
			wnd.LoadingComplete();
			break;
		}
		default:break;
		}
	}

	if (wnd.ResizeCalled())
	{
		rg.reset();
		wnd.Gfx().OnResize(wnd.GetWidth(), wnd.GetHeight());
		CreateRenderGraph();
		wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(wnd.GetHeight()) / float(wnd.GetWidth()), 0.5f, 100.0f));
		wnd.ResizeComplete();
	}

	if (wnd.RestyleCalled())
	{
		imgui.SetStyle((ImGUIManager::Style(wnd.GetStyle())));
		wnd.RestyleComplete();
	}
}

void App::CreateRenderGraph()
{
	rg.emplace(wnd.Gfx());

	//grid.LinkTechniques(*rg);
	sphere.LinkTechniques(*rg);
	//light.LinkTechniques(*rg);
	text.LinkTechniques(*rg);
	//ss.LinkTechniques(*rg);
	if (model) model->LinkTechniques(*rg);
}

winrt::fire_and_forget App::ReloadModelAsync()
{
	co_await winrt::resume_background();
	auto wfilename = opener.GetFilePath();

	if (!wfilename.empty())
	{
		swap = std::make_unique<Model>(wnd.Gfx(), ToNarrow(wfilename));
		//co_await Model::MakeModelAsync(swap, wnd.Gfx(), ToNarrow(wfilename));

		if (!swap) MessageBox(nullptr, "Model file was corrupted or empty",
			"Model Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	state = ModelLoadState::Finish;
}

