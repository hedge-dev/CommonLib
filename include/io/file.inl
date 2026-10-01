#include <array>
#include <bit>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "../thirdparty/simdutf/singleheader/simdutf.h"

#include "hash/hash_provider.h"
#include "mem/mem.h"
#include "ut/expr/string_expr.h"
#include "ut/encoding.h"

#define __CMNLIB_INTERNAL_STATIC_LIB_ENROLMENT

namespace hedgedev::csl::io::file
{
    inline constexpr uint64_t k_optimal_buffer_size = 0x10000;

    template <typename T>
    inline bool try_compute_hash(hash::hash_provider<T>& in_hash_provider, std::ifstream& in_file, T& out_hash, bool in_from_beginning, hash_progress_callback_t<T> in_callback)
    {
        bool result{};

        if (!in_file.is_open())
            return result;

        const auto start_pos = uint64_t(in_file.tellg());

        in_file.seekg(0, std::ios::end);

        const auto length = uint64_t(in_file.tellg());

        in_file.seekg(in_from_beginning ? 0 : start_pos, std::ios::beg);
        
        char buffer[k_optimal_buffer_size]{};

        while (in_file)
        {
            in_file.read(buffer, sizeof(buffer));

            const auto count = uint64_t(in_file.gcount());

            if (!count)
                break;

            in_hash_provider.update(reinterpret_cast<uint8_t*>(buffer), count);

            // Exit hash loop if the callback returned false.
            if (in_callback && !in_callback(in_hash_provider.digest(), uint64_t(in_file.tellg()), length))
            {
                result = false;
                break;
            }

            result = true;
        }

        in_file.clear();
        in_file.seekg(start_pos, std::ios::beg);

        out_hash = in_hash_provider.digest();

        return result;
    }
    
    template <hash::hash_provider_t T>
    inline bool try_compute_hash(std::ifstream& in_file, typename T::type& out_hash, bool in_from_beginning, hash_progress_callback_t<typename T::type> in_callback)
    {
        T hash_provider{};

        return try_compute_hash(hash_provider, in_file, out_hash, in_from_beginning, in_callback);
    }

    template <typename T>
    inline bool try_compute_hash(hash::hash_provider<T>& in_hash_provider, const std::filesystem::path& in_path, T& out_hash, hash_progress_callback_t<T> in_callback)
    {
        std::error_code error_code{};

        if (!std::filesystem::exists(in_path, error_code))
            return false;

        auto file = std::ifstream(in_path, std::ios::binary);

        return try_compute_hash(in_hash_provider, file, out_hash, true, in_callback);
    }

    template <hash::hash_provider_t T>
    inline bool try_compute_hash(const std::filesystem::path& in_path, typename T::type& out_hash, hash_progress_callback_t<typename T::type> in_callback)
    {
        T hash_provider{};

        return try_compute_hash(hash_provider, in_path, out_hash, in_callback);
    }
    
    template <hash::hash_provider_t T>
    inline bool compare(std::ifstream& in_left, std::ifstream& in_right, bool in_from_beginning, hash_progress_callback_t<typename T::type> in_left_callback, hash_progress_callback_t<typename T::type> in_right_callback)
    {
        bool result{};

        if (!in_left.is_open() || !in_right.is_open())
            return result;

        const auto left_start_pos = uint64_t(in_left.tellg());
        const auto right_start_pos = uint64_t(in_right.tellg());

        // Seek to the end of the files to get their lengths.
        in_left.seekg(0, std::ios::end);
        in_right.seekg(0, std::ios::end);

        const auto left_length = uint64_t(in_left.tellg());
        const auto right_length = uint64_t(in_right.tellg());

        // Files are the same length, compute hashes for binary comparison.
        if (left_length == right_length)
        {
            in_left.seekg(in_from_beginning ? 0 : left_start_pos, std::ios::beg);
            in_right.seekg(in_from_beginning ? 0 : right_start_pos, std::ios::beg);

            // The interval at which to place hash checkpoints (16 MiB).
            constexpr auto hash_checkpoint_interval = mem::align<uint64_t>(16 * 1024 * 1024, k_optimal_buffer_size);

            // The total number of hash checkpoints to place.
            const auto hash_checkpoint_count = (left_length + hash_checkpoint_interval - 1) / hash_checkpoint_interval;

            // Patent pending™️
            auto hash_checkpoints = std::vector<typename T::type>(hash_checkpoint_count);

            uint64_t hash_checkpoint_index{};

            const auto left_hash_checkpoint_callback = [&](typename T::type in_hash, uint64_t in_processed_bytes, uint64_t in_total_bytes) -> bool
            {
                if ((in_processed_bytes % hash_checkpoint_interval) == 0)
                {
                    // Store current hash at this position to compare earlier in the second file.
                    hash_checkpoints[hash_checkpoint_index] = in_hash;
                    hash_checkpoint_index++;
                }

                if (in_left_callback)
                    return in_left_callback(in_hash, in_processed_bytes, in_total_bytes);

                return true;
            };

            const auto right_hash_checkpoint_callback = [&](typename T::type in_hash, uint64_t in_processed_bytes, uint64_t in_total_bytes) -> bool
            {
                if ((in_processed_bytes % hash_checkpoint_interval) == 0)
                {
                    // Hashes at the same position do not match, abort.
                    if (hash_checkpoints[hash_checkpoint_index] != in_hash)
                        return false;

                    hash_checkpoint_index++;
                }

                if (in_right_callback)
                    return in_right_callback(in_hash, in_processed_bytes, in_total_bytes);

                return true;
            };

            typename T::type left_hash{};

            const auto left_callback = hash_checkpoint_count
                ? left_hash_checkpoint_callback
                : in_left_callback;

            if (try_compute_hash<T>(in_left, left_hash, in_from_beginning, left_callback))
            {
                typename T::type right_hash{};

                const auto right_callback = hash_checkpoint_count
                    ? right_hash_checkpoint_callback
                    : in_right_callback;

                hash_checkpoint_index = 0;

                if (try_compute_hash<T>(in_right, right_hash, in_from_beginning, right_callback))
                    result = left_hash == right_hash;
            }
        }

        in_left.seekg(left_start_pos, std::ios::beg);
        in_right.seekg(right_start_pos, std::ios::beg);

        return result;
    }

    template <hash::hash_provider_t T>
    inline bool compare(const std::filesystem::path& in_left, const std::filesystem::path& in_right, hash_progress_callback_t<typename T::type> in_left_callback, hash_progress_callback_t<typename T::type> in_right_callback)
    {
        std::error_code error_code{};

        if (!std::filesystem::exists(in_left, error_code) || !std::filesystem::exists(in_right, error_code) || error_code)
            return false;

        if (in_left == in_right)
            return true;

        // The files aren't the same length, skip hashing.
        if (std::filesystem::file_size(in_left, error_code) != std::filesystem::file_size(in_right, error_code) || error_code)
            return false;

        auto left = std::ifstream(in_left, std::ios::binary);
        auto right = std::ifstream(in_right, std::ios::binary);
        
        return compare(left, right, true, in_left_callback, in_right_callback);
    }

    inline ut::encoding::encoding_type get_encoding(const std::filesystem::path& in_path, ut::encoding::encoding_type* out_bom, bool in_native)
    {
        auto file = std::ifstream(in_path, std::ios::binary);

        if (!file)
            return ut::encoding::unknown;

        auto buffer = std::vector<char>(k_optimal_buffer_size);

        file.read(buffer.data(), k_optimal_buffer_size);

        const auto length = size_t(file.gcount());

        if (out_bom)
        {
            *out_bom = length
                ? ut::encoding::encoding_type(simdutf::BOM::check_bom(buffer.data(), length))
                : ut::encoding::unknown;
        }

        auto result = length
            ? ut::encoding::encoding_type(simdutf::autodetect_encoding(buffer.data(), length))
            : ut::encoding::unknown;

        if (in_native)
        {
            if (out_bom)
                *out_bom = ut::expr::get_native_encoding_type(*out_bom);

            result = ut::expr::get_native_encoding_type(result);
        }

        return result;
    }

    template <ut::expr::basic_string T>
    inline T read_all_text(const std::filesystem::path& in_path)
    {
        T result{};

        std::error_code error_code{};

        if (!std::filesystem::exists(in_path, error_code))
            return result;

        ut::encoding::encoding_type bom{};
        const auto encoding = get_encoding(in_path, &bom);
        const auto bom_length = ut::encoding::get_bom_length(bom);

        auto file = std::ifstream(in_path, std::ios::binary | std::ios::ate);

        if (!file)
            return result;

        const auto length = int64_t(file.tellg()) - bom_length;

        if (length <= 0)
            return result;

        auto buffer = std::vector<char>(length);

        // Skip the byte order mark, if present.
        file.seekg(bom_length, std::ios::beg);

        // Read file contents into buffer.
        file.rdbuf()->sgetn(buffer.data(), length);

        const auto native_encoding = ut::expr::get_native_encoding_type(encoding);

        switch (ut::expr::get_encoding_char_size(native_encoding))
        {
            case sizeof(char16_t):
                result = ut::encoding::convert<T>(std::u16string_view(reinterpret_cast<const char16_t*>(buffer.data()), length / sizeof(char16_t)), native_encoding, encoding);
                break;

            case sizeof(char32_t):
                result = ut::encoding::convert<T>(std::u32string_view(reinterpret_cast<const char32_t*>(buffer.data()), length / sizeof(char32_t)), native_encoding, encoding);
                break;

            default:
                result = ut::encoding::convert<T>(std::string_view(buffer.data(), length), native_encoding, encoding);
                break;
        }

        return result;
    }

    template <ut::expr::any_string T, ut::encoding::encoding_type target_encoding>
    inline bool write_all_text(const std::filesystem::path& in_path, const T& in_str, bool in_write_bom)
    {
        const auto str = ut::encoding::convert<target_encoding>(in_str);

        if (str.empty())
            return false;

        auto file = std::ofstream(in_path, std::ios::binary);

        if (!file)
            return false;

        if (in_write_bom)
        {
            const auto bom = get_bom(target_encoding);
            file.write(reinterpret_cast<const char*>(bom.data()), bom.size());
        }

        file.write(reinterpret_cast<const char*>(str.data()), str.size() * sizeof(ut::expr::get_encoding_char_type_t<target_encoding>));
        file.close();

        return true;
    }

    template <ut::encoding::encoding_type target_encoding, ut::expr::any_string T>
    inline bool write_all_text(const std::filesystem::path& in_path, const T& in_str, bool in_write_bom)
    {
        return write_all_text<T, target_encoding>(in_path, in_str, in_write_bom);
    }
}
