#pragma once
#include <DirectXTex.h>
#include <Framework/Exception.h> 

class Surface
{
	class LoadException : public Exception
	{
	public:
		LoadException(int line, const char* file, std::string_view filepath, std::string note, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
	private:
		std::string note;
	};
public:
	Surface() = default;
public:
	bool FromFile(std::string_view filepath);
public:
	UINT GetWidth()const noexcept;
	UINT GetHeight()const noexcept;
	UINT GetStride()const noexcept;
	auto* operator ->()
	{
		return &image;
	}
	bool UsesAlpha()const noexcept;
	void* GetBufferPtr()const noexcept;
private:
	DirectX::ScratchImage image;
	static constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
};