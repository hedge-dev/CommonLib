#include <CommonLib.h>
#include <Psapi.h>
#include <shellapi.h>
#include "StackFrameWin32.h"

namespace hedgedev::csl::diag::this_process
{
	template <typename TString = std::wstring>
	inline std::vector<TString> GetCommandLineArguments()
	{
		std::vector<TString> result{};

		if (const auto pCommandLine = GetCommandLineW())
		{
			int argCount{};

			if (const auto ppArguments = CommandLineToArgvW(pCommandLine, &argCount))
			{
				for (auto i = 0; i < argCount; i++)
					result.push_back(ppArguments[i]);
			}
		}

		return result;
	}

	inline std::filesystem::path GetExecutablePath()
	{
		WCHAR buffer[MAX_PATH]{};

		if (!GetModuleFileNameW(NULL, buffer, ARRAYSIZE(buffer)))
			return {};

		return buffer;
	}

	inline std::filesystem::path GetModulePathFromAddress(const void* in_pAddr)
	{
		if (HasAddress(in_pAddr))
			return GetExecutablePath();

		HMODULE hModule{};

		if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)in_pAddr, &hModule))
		{
			WCHAR buffer[MAX_PATH]{};

			if (GetModuleFileNameW(hModule, buffer, ARRAYSIZE(buffer)))
				return buffer;
		}

		return {};
	}

	inline std::filesystem::path GetWorkingDirectory()
	{
		WCHAR buffer[MAX_PATH]{};

		if (!GetCurrentDirectoryW(MAX_PATH, buffer))
			return {};

		return buffer;
	}

	inline std::vector<StackFrame> GetStackTrace(const void* in_pContext, const size_t in_maxFrames)
	{
		std::vector<StackFrame> result{};

		auto context = *(CONTEXT*)in_pContext;

		DWORD machineType{};
		const auto hProcess = GetCurrentProcess();
		const auto hThread = GetCurrentThread();
		STACKFRAME stackFrame{};

#if defined(_M_AMD64)
		machineType = IMAGE_FILE_MACHINE_AMD64;
		stackFrame.AddrPC.Offset = context.Rip;
		stackFrame.AddrPC.Mode = AddrModeFlat;
		stackFrame.AddrStack.Offset = context.Rsp;
		stackFrame.AddrStack.Mode = AddrModeFlat;
		stackFrame.AddrFrame.Offset = context.Rbp;
		stackFrame.AddrFrame.Mode = AddrModeFlat;
#elif defined(_M_IX86)
		machineType = IMAGE_FILE_MACHINE_I386;
		stackFrame.AddrPC.Offset = context.Eip;
		stackFrame.AddrPC.Mode = AddrModeFlat;
		stackFrame.AddrStack.Offset = context.Esp;
		stackFrame.AddrStack.Mode = AddrModeFlat;
		stackFrame.AddrFrame.Offset = context.Ebp;
		stackFrame.AddrFrame.Mode = AddrModeFlat;
#else
		return result;
#endif

		auto upSymbolInfoBuffer = std::make_unique<uint8_t[]>(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(CHAR));
		auto pSymbolInfo = (SYMBOL_INFO*)upSymbolInfoBuffer.get();
		pSymbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbolInfo->MaxNameLen = MAX_SYM_NAME;

		IMAGEHLP_LINE lineInfo{ sizeof(IMAGEHLP_LINE) };

		size_t frameIndex{};

		while (frameIndex < in_maxFrames || !in_maxFrames)
		{
			auto status = StackWalk
			(
				machineType, hProcess, hThread, &stackFrame, &context,
				NULL, SymFunctionTableAccess, SymGetModuleBase, NULL
			);

			if (!status || !stackFrame.AddrPC.Offset)
				break;

			StackFrameWin32 currentFrame{};

			currentFrame.SetModulePath(GetModulePathFromAddress((void*)stackFrame.AddrPC.Offset));
			currentFrame.pProgramCounter = (void*)stackFrame.AddrPC.Offset;

			size_t displacement{};

			if (SymFromAddr(hProcess, stackFrame.AddrPC.Offset, (PDWORD64)&displacement, pSymbolInfo))
			{
				auto dwDisplacement = DWORD(displacement);

				if (SymGetLineFromAddr(hProcess, stackFrame.AddrPC.Offset, &dwDisplacement, &lineInfo))
					currentFrame.SetLineInfo(&lineInfo);

				currentFrame.SetSymbolInfo(pSymbolInfo, displacement);
			}

			result.push_back(currentFrame);

			frameIndex++;
		}

		return result;
	}

	inline bool HasAddress(const void* in_pAddr)
	{
		const auto hMainModule = GetModuleHandle(NULL);

		if (!hMainModule)
			return false;

		MODULEINFO mainModuleInfo{};
		GetModuleInformation(GetCurrentProcess(), hMainModule, &mainModuleInfo, sizeof(MODULEINFO));

		const auto addr = size_t(in_pAddr);
		const auto start = size_t(mainModuleInfo.lpBaseOfDll);
		const auto end = size_t(start + mainModuleInfo.SizeOfImage);

		return addr >= start && addr < end;
	}

	template <typename TString = std::wstring_view>
	inline bool Restart(const std::vector<TString>& in_rArgs)
	{
		process::Start(GetExecutablePath(), in_rArgs, GetWorkingDirectory());

		return TerminateProcess(GetCurrentProcess(), 0);
	}

	inline bool Restart()
	{
		return Restart({});
	}
}
