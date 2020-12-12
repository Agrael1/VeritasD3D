#pragma once

#include "MainPage.g.h"
#include "Graphics.h"

namespace winrt::Veritas_3D_View::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
        void TabView_TabCloseRequested(Microsoft::UI::Xaml::Controls::TabView sender, Microsoft::UI::Xaml::Controls::TabViewTabCloseRequestedEventArgs args);
        void cameraButton_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void lightningButton_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void translationButton_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void cameraButton_Unchecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void CloseSelectedTabKeyboardAccelerator_Invoked(winrt::Windows::UI::Xaml::Input::KeyboardAccelerator const& sender, winrt::Windows::UI::Xaml::Input::KeyboardAcceleratorInvokedEventArgs const& args);
    };
}

namespace winrt::Veritas_3D_View::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
