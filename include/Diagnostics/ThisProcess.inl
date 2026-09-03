#include <CommonLib.h>

namespace hedgedev::csl::diag::this_process
{
	inline std::filesystem::path GetExecutableRoot()
	{
		return GetExecutablePath().remove_filename();
	}

	inline bool IsNeighbour(const std::filesystem::path& in_rPath)
	{
		return ut::filesystem::IsNeighbour(GetExecutablePath(), in_rPath);
	}
}
