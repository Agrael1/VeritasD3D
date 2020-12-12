#pragma once

#include "TranslationPage.g.h"

namespace winrt::Veritas_3D_View::implementation
{
    struct TranslationPage : TranslationPageT<TranslationPage>
    {
        TranslationPage();

    };
}

namespace winrt::Veritas_3D_View::factory_implementation
{
    struct TranslationPage : TranslationPageT<TranslationPage, implementation::TranslationPage>
    {
    };
}
