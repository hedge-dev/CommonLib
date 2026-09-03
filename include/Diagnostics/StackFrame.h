#pragma once

#include <filesystem>

#include "LineInfo.h"
#include "SymbolInfo.h"

namespace hedgedev::csl::diag
{
    class StackFrame
    {
    protected:
        std::filesystem::path m_modulePath{};
        std::filesystem::path m_canonicalModulePath{};

        bool m_hasSymbolInfo{};
        bool m_hasLineInfo{};

    public:
        void* pProgramCounter{};
        void* pModuleBase{};

        SymbolInfo SymbolInfo{};
        LineInfo LineInfo{};

        virtual const std::filesystem::path& GetModulePath(bool in_isCanonical = true) const
        {
            return in_isCanonical ? m_canonicalModulePath : m_modulePath;
        }

        virtual void SetModulePath(const std::filesystem::path& in_rPath)
        {
            std::error_code errorCode{};

            m_modulePath = in_rPath;
            m_canonicalModulePath = std::filesystem::canonical(m_modulePath, errorCode);

            if (!errorCode)
                return;

            m_canonicalModulePath = m_modulePath;
        }

        virtual bool HasSymbolInfo() const
        {
            return m_hasSymbolInfo;
        }

        virtual bool HasLineInfo() const
        {
            return m_hasLineInfo;
        }
    };
}
