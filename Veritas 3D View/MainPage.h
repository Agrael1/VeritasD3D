#pragma once

#include "MainPage.g.h"

namespace winrt::Veritas_3D_View::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
    };
}

namespace winrt::Veritas_3D_View::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
