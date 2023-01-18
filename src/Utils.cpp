#include <GalaMake/Utils.hpp>

// Timer
void Timer::Start() {
    clock_start = clock();
}

double Timer::Stop() {
    clock_end = clock();

    return (clock_end - clock_start) / (double)CLOCKS_PER_SEC;
}

Timer::Timer() {}

// Etc..
std::vector<std::string> ScanResources(const json &buildConfig, bool jsonCheck) {
    std::vector<std::string> out;

    for(auto &[typeStr, resType] : g_typeStrs) {
        const std::string dir =
            (buildConfig["build_options"]["workspace_dir"]).get<std::string>() +
            (buildConfig["asset_paths"][typeStr + "s"]).get<std::string>();

        if(!(std::filesystem::exists(dir) && std::filesystem::is_directory(dir)))
            continue;

        for(auto &p : std::filesystem::directory_iterator(dir)) {
            if(!std::filesystem::is_directory(p)) continue;

            if(jsonCheck) {
                if(!std::filesystem::exists(p.path().string() + "/resource.json")) continue;
            }

            const auto resName  = p.path().filename().string();
            const bool doQuotes = (resName.find(' ') != std::string::npos);

            if(doQuotes) out.push_back("\"" + typeStr + ":" + resName + "\"");
            else         out.push_back(typeStr + ":" + resName);
        }
    }

    return out;
}

std::pair<std::string, std::string> SplitResourceURI(const std::string &uri) {
    const size_t colonPos = uri.find_first_of(':');

    return {uri.substr(0, colonPos), uri.substr(colonPos + 1)};
}