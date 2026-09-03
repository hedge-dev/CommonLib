#include <CommonLib.h>

namespace hedgedev::csl::ut::filesystem
{
	inline bool IsNeighbour(const std::filesystem::path& in_rPathA, const std::filesystem::path& in_rPathB)
	{
		return string::Compare(in_rPathA.parent_path().c_str(), in_rPathB.parent_path().c_str(), false);
	}

	inline std::error_code TruncateFilesByAge(const std::filesystem::path& in_rPath, std::string_view in_extension, size_t in_max)
	{
		std::error_code result{};

		// Sort files by oldest to newest.
		return TruncateFiles(in_rPath, in_extension, in_max, [&](const auto& a, const auto& b)
		{
			return std::filesystem::last_write_time(a, result) < std::filesystem::last_write_time(b, result);
		});
	}

	template <typename T>
	inline std::error_code TruncateFiles(const std::filesystem::path& in_rPath, std::string_view in_extension, size_t in_max, T&& in_rrCompare)
	{
		std::error_code result{};

		if (!std::filesystem::exists(in_rPath, result) || !std::filesystem::is_directory(in_rPath, result))
			return result;

		std::vector<std::filesystem::directory_entry> files{};

		for (const auto& entry : std::filesystem::directory_iterator(in_rPath))
		{
			if (!entry.is_regular_file(result))
				continue;

			if (entry.path().extension() != in_extension)
				continue;

			files.push_back(entry);
		}

		if (files.size() <= in_max)
			return result;

		std::sort(files.begin(), files.end(), in_rrCompare);

		auto truncateLength = files.size() - in_max;

		for (size_t i = 0; i < truncateLength; i++)
			std::filesystem::remove(files[i].path(), result);

		return result;
	}
}
