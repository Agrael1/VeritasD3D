#include <Shared/DXGIInfoManager.h>
#include <Shared/Definitions.h>
#include <memory>
#include <dxgi1_3.h>

#if DX12_BACKEND
#include <d3d12sdklayers.h>
#else
#include <d3d11sdklayers.h>
#endif

using namespace ver;


struct ID3D12InfoQueue;
struct ID3D11InfoQueue;
enum D3D12_MESSAGE_SEVERITY;
enum D3D11_MESSAGE_SEVERITY;
namespace
{
	using InfoQueue = std::conditional_t<ver::dx12_enabled, ID3D12InfoQueue, ID3D11InfoQueue>;
	using InfoQueueSeverity = std::conditional_t<ver::dx12_enabled, D3D12_MESSAGE_SEVERITY, D3D11_MESSAGE_SEVERITY>;
}


winrt::com_ptr<IDXGIInfoQueue> DXGIInfoManager::info_queue{};

constexpr Severity Convert(DXGI_INFO_QUEUE_MESSAGE_SEVERITY sev)noexcept
{
	using enum Severity;
	switch (sev)
	{
	default:
	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION:
		return critical;
	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR:
		return error;
	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING:
		return warn;
	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO:
		return info;
	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE:
		return debug;
	}
}


DXGIInfoManager::DXGIInfoManager()
{
	winrt::check_hresult(DXGIGetDebugInterface1(0, __uuidof(IDXGIInfoQueue), info_queue.put_void()));

	info_queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
	info_queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
	info_queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, true);

	if (auto d3dinfoqueue = info_queue.try_as<ID3D11InfoQueue>())
	{
		d3dinfoqueue->SetBreakOnSeverity(InfoQueueSeverity(0), true); //Corruption
		d3dinfoqueue->SetBreakOnSeverity(InfoQueueSeverity(1), true); //Error
		d3dinfoqueue->SetBreakOnSeverity(InfoQueueSeverity(2), true); //Warning
	}
}
DXGIInfoManager::~DXGIInfoManager()
{
	info_queue = nullptr;
}
uint64_t DXGIInfoManager::GetNumMessages()noexcept
{
	if (!info_queue)return 0;
	assert(info_queue);
	return info_queue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}
std::vector<DXGIMessage> DXGIInfoManager::GetMessages()noexcept
{
	if (!info_queue)return {};
	std::vector<DXGIMessage> messages;
	const auto end = info_queue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	if (!end)return{};

	for (UINT64 i = 0; i < end; i++)
	{
		SIZE_T messageLength = 0;
		// get the size of message[i]
		winrt::check_hresult(info_queue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// get message and bush it into vector
		winrt::check_hresult(info_queue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(Convert(pMessage->Severity), pMessage->pDescription);
	}
	info_queue->ClearStoredMessages(DXGI_DEBUG_ALL);
	return messages;
}


