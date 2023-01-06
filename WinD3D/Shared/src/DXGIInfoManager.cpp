#include <Shared/DXGIInfoManager.h>
#include <memory>
#include <dxgidebug.h>
#include <dxgi1_3.h>

using namespace ver;

winrt::com_ptr<IDXGIInfoQueue> DXGIInfoManager::info_queue{};

Severity Convert(DXGI_INFO_QUEUE_MESSAGE_SEVERITY sev)
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
}
DXGIInfoManager::~DXGIInfoManager()
{
	info_queue = nullptr;
}
uint64_t DXGIInfoManager::GetNumMessages()
{
	assert(info_queue);
	return info_queue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}
std::vector<DXGIMessage> DXGIInfoManager::GetMessages()
{
	assert(info_queue);
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



