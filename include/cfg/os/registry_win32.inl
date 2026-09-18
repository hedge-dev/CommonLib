#include <filesystem>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "ut/expr/expr.h"
#include "ut/expr/string_types.h"
#include "ut/string.h"

namespace hedgedev::csl::cfg::registry
{
    inline bool _try_parse_registry_path(const std::filesystem::path& in_path, HKEY& out_key, std::wstring& out_sub_key)
    {
        if (in_path.empty())
            return false;

        const auto root = (*in_path.begin()).wstring();

        if (root == L"HKEY_CLASSES_ROOT" || root == L"HKCR")
        {
            out_key = HKEY_CLASSES_ROOT;
        }
        else if (root == L"HKEY_CURRENT_CONFIG" || root == L"HKCC")
        {
            out_key = HKEY_CURRENT_CONFIG;
        }
        else if (root == L"HKEY_CURRENT_USER" || root == L"HKCU")
        {
            out_key = HKEY_CURRENT_USER;
        }
        else if (root == L"HKEY_LOCAL_MACHINE" || root == L"HKLM")
        {
            out_key = HKEY_LOCAL_MACHINE;
        }
        else if (root == L"HKEY_USERS")
        {
            out_key = HKEY_USERS;
        }
        else
        {
            return false;
        }

        out_sub_key = std::filesystem::relative(in_path, root);

        return true;
    }

    template <typename T>
    inline bool read(const std::filesystem::path& in_path, const std::filesystem::path& in_value_name, T& out_result)
    {
        HKEY root_key{};
        std::wstring sub_key{};

        if (!_try_parse_registry_path(in_path, root_key, sub_key))
            return false;

        HKEY key{};

        if (RegOpenKeyExW(root_key, sub_key.c_str(), 0, KEY_READ, &key) != ERROR_SUCCESS)
            return false;

        DWORD buffer_size{};
        DWORD data_type{};

        auto result = RegGetValueW(key, NULL, in_value_name.c_str(), RRF_RT_ANY, &data_type, NULL, &buffer_size);

        if (result != ERROR_SUCCESS)
        {
            RegCloseKey(key);
            return false;
        }

        result = ERROR_INVALID_FUNCTION;

        if constexpr (std::is_same_v<T, std::string>)
        {
            if (data_type == REG_SZ)
            {
                std::vector<uint8_t> buffer{};
                buffer.reserve(buffer_size);

                result = RegGetValueW(key, NULL, in_value_name.c_str(), RRF_RT_REG_SZ, NULL, buffer.data(), &buffer_size);

                if (result == ERROR_SUCCESS)
                {
                    result = ut::string::try_convert(buffer.data(), out_result)
                        ? ERROR_SUCCESS
                        : -1;
                }
            }
        }
        else if constexpr (std::is_same_v<T, std::wstring> || std::is_same_v<T, std::filesystem::path>)
        {
            if (data_type == REG_SZ)
            {
                std::vector<uint8_t> buffer{};
                buffer.reserve(buffer_size);

                result = RegGetValueW(key, NULL, in_value_name.c_str(), RRF_RT_REG_SZ, NULL, buffer.data(), &buffer_size);

                if (result == ERROR_SUCCESS)
                    out_result = reinterpret_cast<wchar_t*>(buffer.data());
            }
        }
        else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>)
        {
            result = RegGetValueW(key, NULL, in_value_name.c_str(), RRF_RT_DWORD, NULL, (BYTE*)&out_result, &buffer_size);
        }
        else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>)
        {
            result = RegGetValueW(key, NULL, in_value_name.c_str(), RRF_RT_QWORD, NULL, (BYTE*)&out_result, &buffer_size);
        }
        else
        {
            static_assert(false, "Unsupported data type.");
        }

        RegCloseKey(key);

        return result == ERROR_SUCCESS;
    }

    template <typename T>
    inline bool read(const std::filesystem::path& in_path, T& out_result)
    {
        return read(in_path.parent_path(), in_path.filename(), out_result);
    }

    template <typename T>
    inline bool write(const std::filesystem::path& in_path, const std::filesystem::path& in_value_name, const T& in_value)
    {
        HKEY root_key{};
        std::wstring sub_key{};

        if (!_try_parse_registry_path(in_path, root_key, sub_key))
            return false;

        HKEY key{};

        if (RegCreateKeyExW(root_key, sub_key.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL) != ERROR_SUCCESS)
            return false;

        BYTE* data{};
        DWORD data_size{};
        DWORD data_type{};
        std::wstring str_data{};
        
        if constexpr (ut::expr::any_string_t<T> || std::is_same_v<T, std::filesystem::path>)
        {
            if (ut::string::try_convert(in_value, str_data))
            {
                data = (BYTE*)str_data.c_str();
                data_size = (str_data.size() + 1) * sizeof(wchar_t);
                data_type = REG_SZ;
            }
            else
            {
                RegCloseKey(key);
                return false;
            }
        }
        else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>)
        {
            data = (BYTE*)&in_value;
            data_size = sizeof(T);
            data_type = REG_DWORD;
        }
        else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>)
        {
            data = (BYTE*)&in_value;
            data_size = sizeof(T);
            data_type = REG_QWORD;
        }
        else
        {
            static_assert(false, "Unsupported data type.");
        }

        const auto result = RegSetValueExW(key, in_value_name.c_str(), 0, data_type, data, data_size);

        RegCloseKey(key);
        
        return result == ERROR_SUCCESS;
    }

    template <typename T>
    inline bool write(const std::filesystem::path& in_path, const T& in_value)
    {
        return write(in_path.parent_path(), in_path.filename(), in_value);
    }
}
