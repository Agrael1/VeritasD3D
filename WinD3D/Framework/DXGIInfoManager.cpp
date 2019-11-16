#include "DXGIInfoManager.h"
#include <Engine/Window.h>
#include <Engine/Graphics.h>
#include <memory>

#include "GraphicsThrows.m"
// GUID manual init (lib seems to be obsolete)

extern "C"
{
	const struct _GUID DXGI_DEBUG_ALL =
	{
		0xe48ae283,
		0xda80,
		0x490b,
		0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x8
	};
};

DXGIInfoManager::DXGIInfoManager()
{
	// define signature of the function
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	const HMODULE hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
		);
	if (DxgiGetDebugInterface == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}
	HRESULT hr;
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), (void**)&pDxgiInfoQueue));
}

void DXGIInfoManager::Set() noexcept
{
	// set the index so we can get messages for this call
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}
std::vector<std::string> DXGIInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (UINT64 i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLength;

		// get the size of message[i]
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// get message and bush it into vector
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}

	return messages;
}

