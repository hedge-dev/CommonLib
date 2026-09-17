#include <filesystem>

namespace hedgedev::csl::diag::this_process
{
	inline std::filesystem::path get_executable_root()
	{
		return get_executable_path().remove_filename();
	}

	inline bool is_neighbour(const std::filesystem::path& in_path)
	{
		return ut::filesystem::is_neighbour(get_executable_path(), in_path);
	}
}
