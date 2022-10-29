#pragma once
#include "Bindable.h"
#include <memory>

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
	IndexBuffer(Graphics& gfx, std::string tag, const std::vector<unsigned short>& indices);
public:
	void Bind(Graphics& gfx) noxnd override;
	UINT GetCount() const noexcept;

	static std::shared_ptr<IndexBuffer> Resolve(Graphics& gfx, const std::string& tag,
		const std::vector<unsigned short>& indices);

	template<typename...Ignore>
	static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
	{
		return GenerateUID_(tag);
	}
	std::string GetUID() const noexcept override;
private:
	static std::string GenerateUID_(const std::string& tag);
protected:
	std::string tag;
	UINT count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
};
