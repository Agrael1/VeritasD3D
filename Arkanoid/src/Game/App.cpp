#include <Game/App.h>

arka::App::App(uint32_t width, uint32_t height)
    : wnd(width, height, "Unreal Tournament"), gfx(width, height)
    , scene(physics, &interaction), ball(gfx, physics, 0.5f), 
    field(gfx, physics)
{
    gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(height) / float(width), 0.5f, 1000.0f));
    wnd.DisableCursor();
    wnd.mouse.EnableRaw();
}
arka::App::~App()
{
}

ver::IAsyncAction arka::App::InitializeAsync()noexcept
{
    co_await gfx.CreateSwapChain(wnd.GetHandle());

    ball.AddToScene(scene);
    field.AddToScene(scene);
}

int arka::App::Go()
{
    float dt = 1.f / 60.f;
    while (true)
    {
        if (const auto a = wnd.ProcessMessages())
            return (int)a.value();
        ProcessEvents();

        scene.get_scene().simulate(dt);
        DoFrame(dt);
        scene.get_scene().fetchResults(true);
        dt = gfx.GetFrameStep();

        ProcessPhysics();
    }
}
void arka::App::ProcessPhysics()
{
    ball.Update();
}

//void App::GameTick()
//{
//    interaction.Apply();
//    if (!block)block = level.PollFinish();
//    float y = player->GetPosition().y;
//    if (y < -300.0f)
//        player->Respawn({ -183.0f, -36.6f, -34.8f });
//}
//
void arka::App::ProcessEvents()
{
    using enum ::Event;
    for (auto e : wnd.GetEvents())
    {
        switch (e)
        {
        case Resize:OnResize(); break;
        case Restyle:OnRestyle(); break;
        default:break;
        }
    }
}
void arka::App::OnResize()
{
    gfx.OnResize(wnd.GetWidth(), wnd.GetHeight());

    CreateRenderGraph();
    gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(wnd.GetHeight()) / float(wnd.GetWidth()), 0.5f, 1000.0f));
    wnd.mouse.BoundCursor(wnd.GetWidth(), wnd.GetHeight());
    gfx.SetCursor(wnd.mouse.GetPos());
}
void arka::App::OnRestyle()
{
    imgui.SetStyle(wnd.GetStyle());
}

void arka::App::DoFrame(float dt)
{
    using namespace DirectX;
    if (!wnd.IsActive())return;

    gfx.BeginFrame(0.2f, 0.2f, 0.2f);
    gfx.SetCentralCamera(camera.GetViewMatrix());
    gfx.SetLeftCamera(camera.GetViewMatrix());
    gfx.SetCamera(camera.GetViewMatrix());

    ball.Submit();
    field.Submit();

    rg->Execute(gfx);
    ProcessInput(dt);
    camera.SpawnControlWindow();

    // Present
    gfx.EndFrame();
    rg->Reset();
}

void arka::App::ProcessInput(float dt)
{
    while (const auto e = wnd.kbd.ReadKey())
    {
        if (!e->IsPress())
            continue;

        switch (e->GetCode())
        {
        case VK_INSERT:
            if (wnd.CursorEnabled())
            {
                wnd.DisableCursor();
                wnd.mouse.EnableRaw();
            }
            else
            {
                wnd.EnableCursor();
                wnd.mouse.DisableRaw();
            }
            break;
        case VK_ESCAPE:
            if (wnd.CursorEnabled())
            {
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
        if (wnd.kbd.KeyIsPressed('A'))
        {
            // Duplicate: move left
        }
        if (wnd.kbd.KeyIsPressed('D'))
        {
            // Duplicate: move right
        }
        if (wnd.kbd.KeyIsPressed(VK_SPACE))
        {
            ball.SetLinearVelocity(0.0f, 4.0f);
        }
    }

    while (const auto delta = wnd.mouse.ReadRawDelta())
    {
        if (!wnd.CursorEnabled())
        {
            // Control the pad
        }
    }

    wnd.kbd.Flush();
    wnd.mouse.Flush();
}

void arka::App::CreateRenderGraph()
{
    rg.emplace(gfx);
    ball.Link(*rg);
    field.Link(*rg);
}

