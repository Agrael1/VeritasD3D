#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
/*#include <WelcomePage.xaml.h>
#include <CameraPage.xaml.h>
#include <LightningPage.xaml.h>
#include <TranslationPage.xaml.h>*/

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::Veritas_3D_View::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
        //show welcome page
        this->WelcomeFrame().Navigate(xaml_typename<WelcomePage>());
        //extend view into title bar
        auto coreTitleBar = Windows::ApplicationModel::Core::CoreApplication::GetCurrentView().TitleBar();
        coreTitleBar.ExtendViewIntoTitleBar(true);
        Window::Current().SetTitleBar(AppTitleBar());
    }



    void MainPage::TabView_TabCloseRequested(Microsoft::UI::Xaml::Controls::TabView sender, Microsoft::UI::Xaml::Controls::TabViewTabCloseRequestedEventArgs args)
    {
        uint32_t index;
        sender.TabItems().IndexOf(args.Tab(), index);
        sender.TabItems().RemoveAt(index);
    }

}


void winrt::Veritas_3D_View::implementation::MainPage::cameraButton_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    this->lightningButton().IsChecked(false);
    this->translationButton().IsChecked(false);

    this->splitView().IsPaneOpen(true);
    this->splitView().OpenPaneLength(324);
    this->SplitFrame().Navigate(xaml_typename<CameraPage>());
}
void winrt::Veritas_3D_View::implementation::MainPage::lightningButton_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    this->cameraButton().IsChecked(false);
    this->translationButton().IsChecked(false);

    this->splitView().IsPaneOpen(true);
    this->splitView().OpenPaneLength(460);
    this->SplitFrame().Navigate(xaml_typename<LightningPage>());
}
void winrt::Veritas_3D_View::implementation::MainPage::translationButton_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    this->cameraButton().IsChecked(false);
    this->lightningButton().IsChecked(false);
    this->splitView().IsPaneOpen(true);
    this->splitView().OpenPaneLength(324);
    this->SplitFrame().Navigate(xaml_typename<TranslationPage>());
}
void winrt::Veritas_3D_View::implementation::MainPage::cameraButton_Unchecked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
{
    //templates are fun
    if (!(this->cameraButton().IsChecked().GetBoolean()) &&
        !(this->lightningButton().IsChecked().GetBoolean()) && !(this->translationButton().IsChecked().GetBoolean())) {
        this->splitView().IsPaneOpen(false);
    }
}

void winrt::Veritas_3D_View::implementation::MainPage::CloseSelectedTabKeyboardAccelerator_Invoked(winrt::Windows::UI::Xaml::Input::KeyboardAccelerator const& sender, winrt::Windows::UI::Xaml::Input::KeyboardAcceleratorInvokedEventArgs const& args)
{
    auto index = this->MainTabView().SelectedIndex();
    this->MainTabView().TabItems().RemoveAt(index);
}