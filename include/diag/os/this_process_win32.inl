#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

#include <Psapi.h>
#include <shellapi.h>

#include "os/win32.h"
#include "ut/expr/string_types.h"
#include "stack_frame_win32.h"

namespace hedgedev::csl::diag::this_process
{
	template <ut::expr::any_string_t T>
	inline std::vector<T> get_command_line()
	{
		std::vector<T> result{};

		if (const auto command_line = GetCommandLineW())
		{
			int arg_count{};

			if (const auto args = CommandLineToArgvW(command_line, &arg_count))
			{
				for (auto i = 0; i < arg_count; i++)
					result.push_back(ut::string::convert<T>(args[i]));
			}
		}

		return result;
	}

	inline std::filesystem::path get_executable_path()
	{
		std::filesystem::path result{};

		WCHAR buffer[MAX_PATH]{};

		if (GetModuleFileNameW(NULL, buffer, ARRAYSIZE(buffer)))
			result = buffer;

		return result;
	}

	inline std::filesystem::path get_module_path_from_address(const void* in_address)
	{
		std::filesystem::path result{};

		if (has_address(in_address))
		{
			result = get_executable_path();
		}
		else
		{
			HMODULE module{};

			if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)in_address, &module))
			{
				WCHAR buffer[MAX_PATH]{};

				if (GetModuleFileNameW(module, buffer, ARRAYSIZE(buffer)))
					result = buffer;
			}
		}

		return result;
	}

	inline std::filesystem::path get_working_directory()
	{
		std::filesystem::path result{};

		WCHAR buffer[MAX_PATH]{};

		if (!GetCurrentDirectoryW(MAX_PATH, buffer))
			result = buffer;

		return result;
	}

	inline std::vector<stack_frame> get_stack_trace(const void* in_context, const size_t in_max_frames)
	{
		std::vector<stack_frame> result{};

		auto context = *(CONTEXT*)in_context;

		DWORD machine_type{};
		const auto current_process = GetCurrentProcess();
		const auto current_thread = GetCurrentThread();
		STACKFRAME stack_frame{};

#if defined(CMNLIB_X64)
		machine_type = IMAGE_FILE_MACHINE_AMD64;
		stack_frame.AddrPC.Offset = context.Rip;
		stack_frame.AddrPC.Mode = AddrModeFlat;
		stack_frame.AddrStack.Offset = context.Rsp;
		stack_frame.AddrStack.Mode = AddrModeFlat;
		stack_frame.AddrFrame.Offset = context.Rbp;
		stack_frame.AddrFrame.Mode = AddrModeFlat;
#elif defined(CMNLIB_X86)
		machine_type = IMAGE_FILE_MACHINE_I386;
		stack_frame.AddrPC.Offset = context.Eip;
		stack_frame.AddrPC.Mode = AddrModeFlat;
		stack_frame.AddrStack.Offset = context.Esp;
		stack_frame.AddrStack.Mode = AddrModeFlat;
		stack_frame.AddrFrame.Offset = context.Ebp;
		stack_frame.AddrFrame.Mode = AddrModeFlat;
#else
		return result;
#endif

		auto symbol_info_buffer = std::make_unique<uint8_t[]>(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(CHAR));
		auto symbol_info = (SYMBOL_INFO*)symbol_info_buffer.get();
		symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol_info->MaxNameLen = MAX_SYM_NAME;

		auto line_info = IMAGEHLP_LINE{ sizeof(IMAGEHLP_LINE) };

		size_t frame_index{};

		while (frame_index < in_max_frames || !in_max_frames)
		{
			auto status = StackWalk
			(
				machine_type, current_process, current_thread, &stack_frame, &context,
				NULL, SymFunctionTableAccess, SymGetModuleBase, NULL
			);

			if (!status || !stack_frame.AddrPC.Offset)
				break;

			stack_frame_win32 current_frame{};

			current_frame.set_module_path(get_module_path_from_address((void*)stack_frame.AddrPC.Offset));
			current_frame.program_counter = (void*)stack_frame.AddrPC.Offset;

			uint64_t displacement{};

			if (SymFromAddr(current_process, stack_frame.AddrPC.Offset, (PDWORD64)&displacement, symbol_info))
			{
				auto displacement_u32 = DWORD(displacement);

				if (SymGetLineFromAddr(current_process, stack_frame.AddrPC.Offset, &displacement_u32, &line_info))
					current_frame.set_line_info(&line_info);

				current_frame.set_symbol_info(symbol_info, displacement);
			}

			result.push_back(current_frame);

			frame_index++;
		}

		return result;
	}

	inline bool has_address(const void* in_address)
	{
		const auto main_module = GetModuleHandle(NULL);

		if (!main_module)
			return false;

		MODULEINFO main_module_info{};
		GetModuleInformation(GetCurrentProcess(), main_module, &main_module_info, sizeof(MODULEINFO));

		const auto address = uintptr_t(in_address);
		const auto start = uintptr_t(main_module_info.lpBaseOfDll);
		const auto end = uintptr_t(start + main_module_info.SizeOfImage);

		return address >= start && address < end;
	}

	template <ut::expr::any_string_t T>
	inline bool restart(const std::vector<T>& in_args)
	{
		process::start(get_executable_path(), in_args, get_working_directory());

		return TerminateProcess(GetCurrentProcess(), 0);
	}

	inline bool restart()
	{
		return restart<std::wstring>({});
	}
}
