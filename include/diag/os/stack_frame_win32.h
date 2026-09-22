#pragma once

#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

#include <sstream>
#include <string>

#include "diag/stack_frame.h"
#include "host/common.h"

namespace hedgedev::csl::diag
{
    class stack_frame_win32 : public stack_frame
    {
    public:
        void set_symbol_info(SYMBOL_INFO* in_symbol_info, uint64_t in_displacement)
        {
            if (!in_symbol_info)
                return;

            module_base = reinterpret_cast<void*>(in_symbol_info->ModBase);

            symbol_info.address = reinterpret_cast<void*>(in_symbol_info->Address);

            if (in_symbol_info->NameLen > 0)
                symbol_info.name = std::string(in_symbol_info->Name, in_symbol_info->NameLen);

            symbol_info.flags = in_symbol_info->Flags;
            symbol_info.displacement = in_displacement;

            m_has_symbol_info = true;
        }

        void set_line_info(IMAGEHLP_LINE* in_line_info)
        {
            if (!in_line_info)
                return;

            line_info.source_file_path = in_line_info->FileName;
            line_info.address = reinterpret_cast<void*>(in_line_info->Address);
            line_info.row = in_line_info->LineNumber;

            m_has_line_info = true;
        }

        std::string str() const
        {
            std::stringstream result{};

            const auto& module_path = get_module_path();

            if (!module_path.empty())
                result << module_path.filename().string() << '!';

            if (!symbol_info.name.empty())
            {
                result << symbol_info.name;

                if (symbol_info.displacement > 0)
                    result << '+' << ut::string::hex<std::string>(symbol_info.displacement, 2);
            }
            else
            {
                result << ut::string::hex<std::string>(size_t(program_counter));
            }

            return result.str();
        }
    };
}
