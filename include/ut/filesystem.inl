#include <filesystem>

#include "ut/string.h"

namespace hedgedev::csl::ut::filesystem
{
	inline bool is_neighbour(const std::filesystem::path& in_left, const std::filesystem::path& in_right)
	{
		return string::compare(in_left.parent_path().c_str(), in_right.parent_path().c_str(), false);
	}

	template <typename T>
	inline std::error_code truncate_files(const std::filesystem::path& in_path, std::string_view in_extension, size_t in_max, T&& in_compare_func)
	{
		std::error_code result{};

		if (!std::filesystem::exists(in_path, result) || !std::filesystem::is_directory(in_path, result))
			return result;

		std::vector<std::filesystem::directory_entry> files{};

		for (const auto& entry : std::filesystem::directory_iterator(in_path))
		{
			if (!entry.is_regular_file(result))
				continue;

			if (entry.path().extension() != in_extension)
				continue;

			files.push_back(entry);
		}

		if (files.size() <= in_max)
			return result;

		std::sort(files.begin(), files.end(), in_compare_func);

		auto truncate_length = files.size() - in_max;

		for (size_t i = 0; i < truncate_length; i++)
			std::filesystem::remove(files[i].path(), result);

		return result;
	}

	inline std::error_code truncate_files_by_age(const std::filesystem::path& in_path, std::string_view in_extension, size_t in_max)
	{
		std::error_code result{};

		// Sort files by oldest to newest.
		return truncate_files(in_path, in_extension, in_max, [&](const auto& in_left, const auto& in_right) -> bool
		{
			return std::filesystem::last_write_time(in_left, result) < std::filesystem::last_write_time(in_right, result);
		});
	}
}
