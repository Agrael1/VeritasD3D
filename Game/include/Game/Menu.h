#pragma once
#include <wil/resource.h>
#include <Engine/Util/ImGUIManager.h>

namespace UT
{
	class Menu
	{
	public:
		using Style = ImGUIManager::Style;
		enum class Mode
		{
			Image,
			Video,
			Model,
			Game
		};
	public:
		Menu() = default;
	public:
		void Initialize(HMENU menu)noexcept
		{
			main_menu.reset(menu);
			file.reset(GetSubMenu(menu, 0));
			view.reset(GetSubMenu(menu, 1));
			mode.reset(GetSubMenu(menu, 2));
			styles.reset(GetSubMenu(view.get(), 1));
			CheckMenuRadioItem(styles.get(), 0, 2, std::to_underlying(ui_style), MF_BYPOSITION);
		}
		void EnableLoading()noexcept
		{
			EnableMenuItem(file.get(), 0, MF_BYPOSITION | MF_ENABLED);
		}
		void DisableLoading()noexcept
		{
			EnableMenuItem(file.get(), 0, MF_BYPOSITION | MF_DISABLED);
		}
		void ToggleGrid()noexcept
		{
			grid_enabled ^= 1;
			MENUITEMINFO mii
			{
				.cbSize = sizeof(MENUITEMINFO),
				.fMask = MIIM_STATE,
				.fState = UINT(grid_enabled ? MFS_CHECKED : MFS_UNCHECKED)
			};
			SetMenuItemInfo(view.get(), 0, true, &mii);
		}
		// returns true if restyle is needed
		bool SetStyle(ImGUIManager::Style xui_style)noexcept
		{
			if (ui_style != xui_style)
			{
				ui_style = xui_style;
				CheckMenuRadioItem(styles.get(), 0, 2, std::to_underlying(ui_style), MF_BYPOSITION);
				return true;
			}
			return false;
		}
		auto GetStyle()const noexcept { return ui_style; }
		bool GridEnabled()const noexcept { return grid_enabled; }
	private:
		wil::unique_hmenu main_menu;
		wil::unique_hmenu file;
		wil::unique_hmenu view;
		wil::unique_hmenu styles;
		wil::unique_hmenu mode;

		ImGUIManager::Style ui_style = ImGUIManager::Style::Dark;
		bool grid_enabled = false;
	};
}