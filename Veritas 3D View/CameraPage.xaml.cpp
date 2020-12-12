#include "pch.h"
#include "CameraPage.xaml.h"
#if __has_include("CameraPage.g.cpp")
#include "CameraPage.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::Veritas_3D_View::implementation
{
    CameraPage::CameraPage()
    {
        InitializeComponent();
    }
    int32_t CameraPage::MyProperty()
    {
        return int32_t();
    }
    void CameraPage::MyProperty(int32_t value)
    {
    }
}
