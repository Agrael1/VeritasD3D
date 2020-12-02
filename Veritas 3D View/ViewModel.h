#pragma once

#include "ViewModel.g.h"

namespace winrt::Veritas_3D_View::implementation
{
    struct ViewModel : ViewModelT<ViewModel>
    {
        ViewModel() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::Veritas_3D_View::factory_implementation
{
    struct ViewModel : ViewModelT<ViewModel, implementation::ViewModel>
    {
    };
}
