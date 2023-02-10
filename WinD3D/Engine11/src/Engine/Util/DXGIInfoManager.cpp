#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <memory>
#include <dxgidebug.h>

using namespace ver;

winrt::com_ptr<IDXGIInfoQueue> DXGIInfoManager::info_queue{};

DXGIInfoManager::DXGIInfoManager()
{
	// define signature of the function
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	const HMODULE hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
		throw ver::make_error<ver::hr_error>({ (HRESULT)GetLastError() });

	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
		);

	if (DxgiGetDebugInterface == nullptr)
		throw ver::make_error<ver::hr_error>({ (HRESULT)GetLastError() });

	winrt::check_hresult(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), info_queue.put_void()));
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
std::vector<std::string> DXGIInfoManager::GetMessages()
{
	assert(info_queue);
	std::vector<std::string> messages;
	const auto end = info_queue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	if (!end)return{};

	for (UINT64 i = 0; i < end; i++)
	{
		SIZE_T messageLength = 0;
		// get the size of message[i]
		ver::check_hresult(info_queue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// get message and bush it into vector
		ver::check_hresult(info_queue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}
	info_queue->ClearStoredMessages(DXGI_DEBUG_ALL);
	return messages;
}

