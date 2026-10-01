#pragma once

#include <bit>
#include <filesystem>
#include <fstream>
#include <functional>

#include "hash/hash.h"
#include "hash/hash_provider.h"
#include "hash/xxhash64_provider.h"
#include "ut/expr/string_expr.h"
#include "ut/encoding.h"

namespace hedgedev::csl::io::file
{
    template <typename T>
    using hash_progress_callback_t = std::function<bool(T in_hash, uint64_t in_processed_bytes, uint64_t in_total_bytes)>;

    ///
    /// Computes a file hash.
    ///
    /// \param in_hash_provider  The provider of the hash for the file.
    /// \param in_path           The file to hash.
    /// \param out_hash          The hash of the file.
    /// \param in_from_beginning Determines whether to seek to the beginning of the
    ///                          stream before computing the hash. The original position
    ///                          will be restored after completion.
    ///
    /// \returns `true` if the hash was computed successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool try_compute_hash(hash::hash_provider<T>& in_hash_provider, std::ifstream& in_file, T& out_hash, bool in_from_beginning = true, hash_progress_callback_t<T> in_callback = nullptr);

    ///
    /// Computes a file hash.
    ///
    /// \tparam T The provider of the hash for the file. Uses xxHash64 by default.
    ///
    /// \param in_file           The file to hash.
    /// \param out_hash          The hash of the file.
    /// \param in_from_beginning Determines whether to seek to the beginning of the
    ///                          stream before computing the hash. The original position
    ///                          will be restored after completion.
    ///
    /// \returns `true` if the hash was computed successfully. Otherwise, `false`.
    ///
    template <hash::hash_provider_t T = hash::xxhash64_provider>
    inline bool try_compute_hash(std::ifstream& in_file, typename T::type& out_hash, bool in_from_beginning = true, hash_progress_callback_t<typename T::type> in_callback = nullptr);

    ///
    /// Computes a file hash.
    ///
    /// \param in_hash_provider The provider of the hash for the file.
    /// \param in_path          The path to the file to hash.
    /// \param out_hash         The hash of the file.
    ///
    /// \returns `true` if the hash was computed successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool try_compute_hash(hash::hash_provider<T>& in_hash_provider, const std::filesystem::path& in_path, T& out_hash, hash_progress_callback_t<T> in_callback = nullptr);

    ///
    /// Computes a file hash.
    ///
    /// \tparam T The provider of the hash for the file. Uses xxHash64 by default.
    ///
    /// \param in_path  The path to the file to hash.
    /// \param out_hash The hash of the file.
    ///
    /// \returns `true` if the hash was computed successfully. Otherwise, `false`.
    ///
    template <hash::hash_provider_t T = hash::xxhash64_provider>
    inline bool try_compute_hash(const std::filesystem::path& in_path, typename T::type& out_hash, hash_progress_callback_t<typename T::type> in_callback = nullptr);

    ///
    /// Compares the contents of two files.
    ///
    /// \tparam T The provider of the hashes for the files. Uses xxHash64 by default.
    ///
    /// \param in_left           The first file.
    /// \param in_right          The second file.
    /// \param in_from_beginning Determines whether to seek to the beginning of the
    ///                          stream before computing the hash. The original position
    ///                          will be restored after completion.
    ///
    /// \returns `true` if the files' contents are identical. Otherwise, `false`.
    ///
    template <hash::hash_provider_t T = hash::xxhash64_provider>
    inline bool compare(std::ifstream& in_left, std::ifstream& in_right, bool in_from_beginning = true, hash_progress_callback_t<typename T::type> in_left_callback = nullptr, hash_progress_callback_t<typename T::type> in_right_callback = nullptr);

    ///
    /// Compares the contents of two files.
    ///
    /// \tparam T The provider of the hashes for the files. Uses xxHash64 by default.
    ///
    /// \param in_left  The first file.
    /// \param in_right The second file.
    ///
    /// \returns `true` if the files exist, and their paths or contents are identical. Otherwise, `false`.
    ///
    template <hash::hash_provider_t T = hash::xxhash64_provider>
    inline bool compare(const std::filesystem::path& in_left, const std::filesystem::path& in_right, hash_progress_callback_t<typename T::type> in_left_callback = nullptr, hash_progress_callback_t<typename T::type> in_right_callback = nullptr);

    ///
    /// Gets the text encoding of a file heuristically.
    ///
    /// \param in_path   The path to the file to check.
    /// \param out_bom   The byte order mark, if present.
    /// \param in_native Determines whether to transform the encoding type to the
    ///                  native endianness.
    ///
    /// \returns The encoding of the file.
    ///
    inline ut::encoding::encoding_type get_encoding(const std::filesystem::path& in_path, ut::encoding::encoding_type* out_bom = nullptr, bool in_native = false);

    ///
    /// Reads a text file into a string.
    /// 
    /// \tparam T The string type. If this doesn't match the file encoding, the text
    ///           will be converted automatically.
    /// 
    /// \param in_path The path to the file to read.
    /// 
    /// \returns The text from the file in a new string.
    ///
    template <ut::expr::basic_string T>
    inline T read_all_text(const std::filesystem::path& in_path);

    ///
    /// Writes a string to a file.
    /// 
    /// \tparam T               The string type.
    /// \tparam target_encoding The encoding of the file inferred from the string type.
    ///
    /// \param in_path      The path to the file to write.
    /// \param in_str       The string to write to the file.
    /// \param in_write_bom Determines whether to write a byte order mark at the
    ///                     beginning of the file. This behaviour is automatically
    ///                     determined by the encoding type, but can be overridden
    ///                     if needed. Avoid writing a byte order mark for UTF-8,
    ///                     as it is unnecessary.
    ///
    /// \returns `true` if the file was written successfully. Otherwise, `false`.
    ///
    template <ut::expr::any_string T, ut::encoding::encoding_type target_encoding = ut::expr::get_char_encoding_type<T>()>
    inline bool write_all_text(const std::filesystem::path& in_path, const T& in_str, bool in_write_bom = target_encoding != ut::encoding::utf8);

    ///
    /// Writes a string to a file.
    /// 
    /// \tparam target_encoding The encoding of the file.
    /// \tparam T               The string type.
    ///
    /// \param in_path      The path to the file to write.
    /// \param in_str       The string to write to the file.
    /// \param in_write_bom Determines whether to write a byte order mark at the
    ///                     beginning of the file. This behaviour is automatically
    ///                     determined by the encoding type, but can be overridden
    ///                     if needed. Avoid writing a byte order mark for UTF-8,
    ///                     as it is unnecessary.
    ///
    /// \returns `true` if the file was written successfully. Otherwise, `false`.
    ///
    template <ut::encoding::encoding_type target_encoding, ut::expr::any_string T>
    inline bool write_all_text(const std::filesystem::path& in_path, const T& in_str, bool in_write_bom = target_encoding != ut::encoding::utf8);
}

#include "file.inl"
