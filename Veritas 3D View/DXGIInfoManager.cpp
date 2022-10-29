#include "pch.h"
#include "DXGIInfoManager.h"
#include <memory>


DXGIInfoManager::DXGIInfoManager()
{
	winrt::check_hresult(DXGIGetDebugInterface1(0, __uuidof(IDXGIInfoQueue), (void**)&pDxgiInfoQueue));
}

void DXGIInfoManager::Set() noexcept
{
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}
winrt::hstring DXGIInfoManager::GetMessages() const
{
	winrt::hstring messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (UINT64 i = next; i < end; i++)
	{
		SIZE_T messageLength = 0;

		// get the size of message[i]
		winrt::check_hresult(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// get message and bush it into vector
		winrt::check_hresult(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages = messages + winrt::hstring(winrt::to_hstring(pMessage->pDescription));
	}

	return messages;
}