#pragma once

#include "CameraPage.g.h"

namespace winrt::Veritas_3D_View::implementation
{
    struct CameraPage : CameraPageT<CameraPage>
    {
        CameraPage();
        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::Veritas_3D_View::factory_implementation
{
    struct CameraPage : CameraPageT<CameraPage, implementation::CameraPage>
    {
    };
}
