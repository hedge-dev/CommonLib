#pragma once

#include <filesystem>
#include <sstream>
#include <string>

#include <CommonLib.h>

namespace hedgedev::csl::diag
{
    class StackFrameWin32 : public StackFrame
    {
    public:
        void SetSymbolInfo(SYMBOL_INFO* in_pSymbolInfo, size_t in_displacement)
        {
            if (!in_pSymbolInfo)
                return;

            pModuleBase = (void*)in_pSymbolInfo->ModBase;

            SymbolInfo.pAddress = (void*)in_pSymbolInfo->Address;

            if (in_pSymbolInfo->NameLen > 0)
                SymbolInfo.Name = std::string(in_pSymbolInfo->Name, in_pSymbolInfo->NameLen);

            SymbolInfo.Flags = in_pSymbolInfo->Flags;
            SymbolInfo.Displacement = in_displacement;

            m_hasSymbolInfo = true;
        }

        void SetLineInfo(IMAGEHLP_LINE* in_pLineInfo)
        {
            if (!in_pLineInfo)
                return;

            LineInfo.SourceFilePath = in_pLineInfo->FileName;
            LineInfo.pAddress = (void*)in_pLineInfo->Address;
            LineInfo.Row = in_pLineInfo->LineNumber;

            m_hasLineInfo = true;
        }

        std::string ToString() const
        {
            std::stringstream result{};

            const auto& rModulePath = GetModulePath();

            if (!rModulePath.empty())
                result << rModulePath.filename().string() << '!';

            if (!SymbolInfo.Name.empty())
            {
                result << SymbolInfo.Name;

                if (SymbolInfo.Displacement > 0)
                    result << '+' << ut::string::ToHex<std::string>(SymbolInfo.Displacement, 2);
            }
            else
            {
                result << ut::string::ToHex<std::string>(size_t(pProgramCounter));
            }

            return result.str();
        }
    };
}
