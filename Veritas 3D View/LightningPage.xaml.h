#pragma once

#include "LightningPage.g.h"

namespace winrt::Veritas_3D_View::implementation
{
    struct LightningPage : LightningPageT<LightningPage>
    {
        LightningPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::Veritas_3D_View::factory_implementation
{
    struct LightningPage : LightningPageT<LightningPage, implementation::LightningPage>
    {
    };
}
