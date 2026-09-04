namespace hedgedev::csl::cfg::registry
{
    inline bool TryParseRegistryPath(const std::filesystem::path& in_rPath, HKEY& out_rHKey, std::wstring& out_rSubKey)
    {
        if (in_rPath.empty())
            return false;

        const auto root = (*in_rPath.begin()).wstring();

        if (root == L"HKEY_CLASSES_ROOT" || root == L"HKCR")
        {
            out_rHKey = HKEY_CLASSES_ROOT;
        }
        else if (root == L"HKEY_CURRENT_CONFIG" || root == L"HKCC")
        {
            out_rHKey = HKEY_CURRENT_CONFIG;
        }
        else if (root == L"HKEY_CURRENT_USER" || root == L"HKCU")
        {
            out_rHKey = HKEY_CURRENT_USER;
        }
        else if (root == L"HKEY_LOCAL_MACHINE" || root == L"HKLM")
        {
            out_rHKey = HKEY_LOCAL_MACHINE;
        }
        else if (root == L"HKEY_USERS")
        {
            out_rHKey = HKEY_USERS;
        }
        else
        {
            return false;
        }

        out_rSubKey = std::filesystem::relative(in_rPath, root);

        return true;
    }

    template <typename T>
    inline bool Read(const std::filesystem::path& in_rPath, const std::filesystem::path& in_rValueName, T& out_rData)
    {
        HKEY hRootKey{};
        std::wstring subKey{};

        if (!TryParseRegistryPath(in_rPath, hRootKey, subKey))
            return false;

        HKEY hKey{};

        if (RegOpenKeyExW(hRootKey, subKey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
            return false;

        DWORD bufferSize{};
        DWORD dataType{};

        auto result = RegGetValueW(hKey, nullptr, in_rValueName.c_str(), RRF_RT_ANY, &dataType, nullptr, &bufferSize);

        if (result != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return false;
        }

        result = ERROR_INVALID_FUNCTION;

        if constexpr (std::is_same_v<T, std::string>)
        {
            if (dataType == REG_SZ)
            {
                std::vector<uint8_t> buffer{};
                buffer.reserve(bufferSize);

                result = RegGetValueW(hKey, nullptr, in_rValueName.c_str(), RRF_RT_REG_SZ, nullptr, buffer.data(), &bufferSize);

                if (result == ERROR_SUCCESS)
                {
                    result = hedgedev::csl::ut::string::TryConvert(buffer.data(), out_rData)
                        ? ERROR_SUCCESS
                        : -1;
                }
            }
        }
        else if constexpr (std::is_same_v<T, std::wstring> || std::is_same_v<T, std::filesystem::path>)
        {
            if (dataType == REG_SZ)
            {
                std::vector<uint8_t> buffer{};
                buffer.reserve(bufferSize);

                result = RegGetValueW(hKey, nullptr, in_rValueName.c_str(), RRF_RT_REG_SZ, nullptr, buffer.data(), &bufferSize);

                if (result == ERROR_SUCCESS)
                    out_rData = reinterpret_cast<wchar_t*>(buffer.data());
            }
        }
        else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>)
        {
            result = RegGetValueW(hKey, nullptr, in_rValueName.c_str(), RRF_RT_DWORD, nullptr, (BYTE*)&out_rData, &bufferSize);
        }
        else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>)
        {
            result = RegGetValueW(hKey, nullptr, in_rValueName.c_str(), RRF_RT_QWORD, nullptr, (BYTE*)&out_rData, &bufferSize);
        }
        else
        {
            static_assert(false, "Unsupported data type.");
        }

        RegCloseKey(hKey);

        return result == ERROR_SUCCESS;
    }

    template <typename T>
    inline bool Read(const std::filesystem::path& in_rPath, T& out_rData)
    {
        return Read(in_rPath.parent_path(), in_rPath.filename(), out_rData);
    }

    template <typename T>
    inline bool Write(const std::filesystem::path& in_rPath, const std::filesystem::path& in_rValueName, const T& in_rData)
    {
        HKEY hRootKey{};
        std::wstring subKey{};

        if (!TryParseRegistryPath(in_rPath, hRootKey, subKey))
            return false;

        HKEY hKey{};

        if (RegCreateKeyExW(hRootKey, subKey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
            return false;

        BYTE* pData{};
        DWORD dataSize{};
        DWORD dataType{};
        std::wstring strData{};
        
        if constexpr (hedgedev::csl::ut::expr::IsCString<T>      ||
                      hedgedev::csl::ut::expr::IsCStringW<T>     ||
                      hedgedev::csl::ut::expr::IsStringOrView<T> ||
                      std::is_same_v<T, std::filesystem::path>)
        {
            if (hedgedev::csl::ut::string::TryConvert(in_rData, strData))
            {
                pData = (BYTE*)strData.c_str();
                dataSize = (strData.size() + 1) * sizeof(wchar_t);
                dataType = REG_SZ;
            }
            else
            {
                RegCloseKey(hKey);
                return false;
            }
        }
        else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>)
        {
            pData = (BYTE*)&in_rData;
            dataSize = sizeof(T);
            dataType = REG_DWORD;
        }
        else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>)
        {
            pData = (BYTE*)&in_rData;
            dataSize = sizeof(T);
            dataType = REG_QWORD;
        }
        else
        {
            static_assert(false, "Unsupported data type.");
        }

        auto result = RegSetValueExW(hKey, in_rValueName.c_str(), 0, dataType, pData, dataSize);

        RegCloseKey(hKey);
        
        return result == ERROR_SUCCESS;
    }

    template <typename T>
    inline bool Write(const std::filesystem::path& in_rPath, const T& in_rData)
    {
        return Write(in_rPath.parent_path(), in_rPath.filename(), in_rData);
    }
}
