#pragma once

class ImGUIManager
{
public:
	enum Style
	{
		VGUI,
		Dark,
		Cherry
	};
public:
	ImGUIManager();
	~ImGUIManager();
public:
	void SetStyle(Style s);
	void Begin()const noexcept;
};