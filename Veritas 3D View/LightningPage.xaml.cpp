#include "pch.h"
#include "LightningPage.xaml.h"
#if __has_include("LightningPage.g.cpp")
#include "LightningPage.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::Veritas_3D_View::implementation
{
    LightningPage::LightningPage()
    {
        InitializeComponent();
    }

    int32_t LightningPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void LightningPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}