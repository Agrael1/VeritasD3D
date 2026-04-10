#pragma once
#include <string>
#include <vector>
#include <memory>
#include <span>

class Graphics;

namespace RG
{
	class Source;
	class Sink;

	class Pass
	{
	public:
		explicit Pass(std::string name) noexcept;
		Pass(std::string_view name)noexcept;
		virtual ~Pass();
	public:
		virtual void Execute(Graphics& gfx)const noxnd = 0;
		virtual void Reset()noxnd;
		virtual void Finalize();
		std::string_view GetName() const noexcept;
		std::span<const std::unique_ptr<Sink>> GetSinks()const;
		Source& GetSource(std::string_view registeredName)const;
		Sink& GetSink(std::string_view registeredName)const;
		void SetSinkLinkage(std::string_view registeredName, const std::string& target);
	protected:
		void RegisterSink(std::unique_ptr<Sink> sink);
		void RegisterSource(std::unique_ptr<Source>);
	private:
		std::vector<std::unique_ptr<Sink>> sinks;
		std::vector<std::unique_ptr<Source>> sources;
		std::string name;
	};
}