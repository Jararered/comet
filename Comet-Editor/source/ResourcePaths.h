#pragma once

#include <filesystem>

#if defined(__APPLE__)
#include <cstdint>
#include <limits.h>
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <limits.h>
#endif

inline std::filesystem::path EditorResourcesRoot()
{
    namespace fs = std::filesystem;

    const fs::path cwd = fs::current_path();

    if (fs::is_directory(cwd / "Resources" / "Shaders"))
        return cwd / "Resources";
    if (fs::is_directory(cwd / "Shaders"))
        return cwd;

    std::error_code ec;

#if defined(__APPLE__)
    char buf[PATH_MAX];
    std::uint32_t sz = sizeof(buf);
    if (_NSGetExecutablePath(buf, &sz) == 0)
    {
        fs::path exeDir = fs::weakly_canonical(fs::path(buf), ec).parent_path();
        if (!ec && fs::is_directory(exeDir / "Resources" / "Shaders"))
            return exeDir / "Resources";
    }
#elif defined(__linux__)
    if (fs::path exe = fs::weakly_canonical("/proc/self/exe", ec); !ec)
    {
        fs::path exeDir = exe.parent_path();
        if (fs::is_directory(exeDir / "Resources" / "Shaders"))
            return exeDir / "Resources";
    }
#endif

    return cwd / "Resources";
}
