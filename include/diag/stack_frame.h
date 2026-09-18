#pragma once

#include <filesystem>

#include "line_info.h"
#include "symbol_info.h"

namespace hedgedev::csl::diag
{
    class stack_frame
    {
    protected:
        std::filesystem::path m_module_path{};
        std::filesystem::path m_canonical_module_path{};

        bool m_has_symbol_info{};
        bool m_has_line_info{};

    public:
        void* program_counter{};
        void* module_base{};

        symbol_info symbol_info{};
        line_info line_info{};

        virtual const std::filesystem::path& get_module_path(bool in_canonical = true) const
        {
            return in_canonical ? m_canonical_module_path : m_module_path;
        }

        virtual void set_module_path(const std::filesystem::path& in_path)
        {
            std::error_code error_code{};

            m_module_path = in_path;
            m_canonical_module_path = std::filesystem::canonical(m_module_path, error_code);

            if (!error_code)
                return;

            m_canonical_module_path = m_module_path;
        }

        virtual bool has_symbol_info() const
        {
            return m_has_symbol_info;
        }

        virtual bool has_line_info() const
        {
            return m_has_line_info;
        }
    };
}
