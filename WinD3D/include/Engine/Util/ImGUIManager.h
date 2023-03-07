#pragma once

class ImGUIManager
{
public:
	enum class Style : uint8_t
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