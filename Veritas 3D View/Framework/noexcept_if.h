#pragma once
#define noxnd noexcept(!IS_DEBUG)
#define cnoxnd const noxnd