#pragma once

#include <GalaMake/Common.hpp>
class Timer {
    private:
        clock_t clock_start, clock_end;
    public:
        void Start();
        double Stop();

        Timer();
};

std::vector<std::string> ScanResources(const json &buildConfig, bool jsonCheck = true);

std::pair<std::string, std::string> SplitResourceURI(const std::string &uri);