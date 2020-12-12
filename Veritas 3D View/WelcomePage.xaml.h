#pragma once

#include "WelcomePage.g.h"

namespace winrt::Veritas_3D_View::implementation
{
    struct WelcomePage : WelcomePageT<WelcomePage>
    {
        WelcomePage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        //        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::Veritas_3D_View::factory_implementation
{
    struct WelcomePage : WelcomePageT<WelcomePage, implementation::WelcomePage>
    {
    };
}