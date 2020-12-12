#include "pch.h"
#include "WelcomePage.xaml.h"
#if __has_include("WelcomePage.g.cpp")
#include "WelcomePage.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::Veritas_3D_View::implementation
{
    WelcomePage::WelcomePage()
    {
        InitializeComponent();
    }

    int32_t WelcomePage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void WelcomePage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}