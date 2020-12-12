#include "pch.h"
#include "TranslationPage.xaml.h"
#if __has_include("TranslationPage.g.cpp")
#include "TranslationPage.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::Veritas_3D_View::implementation
{
    TranslationPage::TranslationPage()
    {
        InitializeComponent();
    }
}