#include <GalaMake/Common.hpp>
#include <GalaMake/Paths.hpp>
#include <GalaMake/Building.hpp>
#include <GalaMake/Utils.hpp>
#include <GalaMake/Checking.hpp>
#include <GalaMake/Fixing.hpp>

void PrintError(const ToolError error, const std::vector<std::string> &args = {}) {
    std::cerr << "\e[1;31merror: \e[0m";
    switch(error) {
        case ToolError::InvalidAction:
            if(args.size() >= 1) {
                std::cerr << "invalid action: \"" << args[0] << "\"." << std::endl;
            }else {
                std::cerr << "invalid action." << std::endl;
            }
            break;

        case ToolError::InvalidArgumentCount:
            std::cerr << "invalid argument count." << std::endl;
            break;

        case ToolError::NoFoundConfig:
            std::cerr << "no found build config ('" GALAMAKE_CONFIG_NAME "').\ntry 'galamake new' to create a new build config." << std::endl;
            break;

        case ToolError::InvalidConfig:
            std::cerr << "build config ('" GALAMAKE_CONFIG_NAME "') is invalid.\ntry 'galamake repair' to initiate an attempt at fixing these issues, or 'galamake new' to create a new build config." << std::endl;
            break;

        case ToolError::ResourceNotFound:
            if(args.size() >= 1) {
                std::cerr << "resource not found: \"" << args[0] << "\"." << std::endl;
            }else {
                std::cerr << "resource not found." << std::endl;
            }
            break;

        case ToolError::InvalidResourceType:
            if(args.size() >= 1) {
                std::cerr << "invalid resource type: \"" << args[0] << "\"." << std::endl;
            }else {
                std::cerr << "invalid resource type." << std::endl;
            }
            break;

        case ToolError::InvalidResourceData:
            if(args.size() >= 1) {
                std::cerr << "invalid resource data: \"" << args[0] << "\"." << std::endl;
            }else {
                std::cerr << "invalid resource data." << std::endl;
            }
            break;

        default:
            std::cerr << "UNKNOWN ERROR. THIS IS A BUG." << std::endl;
            break;
    }

    std::cout << "use option '--help' or '-h' for help with using GalaMake." << std::endl;
}

void PrintError(ToolError error, const std::string &arg) {
    std::vector<std::string> args = {arg};
    PrintError(error, args);
}

void PrintVersion() {
    std::cout
        << "GalaMake v1.0.0 - January, 2023\n"
        << "by Colleen (@colleen05), and GitHub contributors.\n"
        << "\n"
        << "This software is distributed under the zlib license.\n"
        << "For more information, see the 'LICENSE' file provided, or visit:\n"
        << "https://choosealicense.com/licenses/zlib/\n"
        << std::endl;
}

void PrintHelp() {
    std::cout
        << "Usage: galamake <action> [args...] [options...]\n"
        << "\n"
        << "Actions:\n"
        << "    new [args...]           Creates new '" GALAMAKE_CONFIG_NAME "' file.\n"
        << "        --default or -d     Skips config setup wizard; creates default config file.\n"
        << "\n"
        << "    build <resource-uri>    Builds resource file of specified resource.\n"
        << "    buildall                Builds project using settings in '" GALAMAKE_CONFIG_NAME "'.\n"
        << "    scan                    Scans and lists each valid resource.\n"
        << "    report                  Scans for and lists missing direcotires and broken resources.\n"
        << "    repair                  Scans for and repairs broken resources and workspace structure.\n"
        << "\n"
        << "Options:\n"
        << "    --version or -v     Display version information.\n"
        << "    --help    or -h     Display this help information.\n"
        << "\n"
        << "Resource URI: <type>:<name>\n"
        << "\n"
        << "Resource Types:\n"
        << "    - texture   Texture\n"
        << "    - sprite    Sprite\n"
        << "    - tileset   Tileset\n"
        << "    - sound     Sound\n"
        << "    - font      Font\n"
        << std::endl;

    PrintVersion();
}

json GetConfigFromSetup(const json &defaults) {
    json j_config = defaults;
    std::string cfg_input;

    // Start message
    std::cout
        << "This utility will walk you through creating a '" GALAMAKE_CONFIG_NAME "' config file.\n"
        << "It only covers some basic, common settings, and further configuration may be required by directly editing the produced file.\n"
        << "\n"
        << "Press ^C at any time to quit.\n"
        << std::endl;



    // Config: Title
    cfg_input.clear();
    std::cout << "Game title (" << j_config["title"] << "): ";
    std::getline(std::cin, cfg_input);
    if(!cfg_input.empty()) j_config["title"] = cfg_input;

    // Config: Description
    cfg_input.clear();
    std::cout << "Game description (" << j_config["description"] << "): ";
    std::getline(std::cin, cfg_input);
    if(!cfg_input.empty()) j_config["description"] = cfg_input;

    // Config: Author
    cfg_input.clear();
    std::cout << "Game author (" << j_config["author"] << "): ";
    std::getline(std::cin, cfg_input);
    if(!cfg_input.empty()) j_config["author"] = cfg_input;

    // Config: Version
    cfg_input.clear();
    std::cout << "Game version (" << j_config["version"] << "): ";
    std::getline(std::cin, cfg_input);
    if(!cfg_input.empty()) j_config["version"] = cfg_input;

    // Config: Workspace directory
    cfg_input.clear();
    std::cout << "Workspace directory (" << j_config["build_options"]["workspace_dir"] << "): ";
    std::getline(std::cin, cfg_input);
    if(!cfg_input.empty()) j_config["build_options"]["workspace_dir"] = cfg_input;

    // Config: Output directory
    cfg_input.clear();
    std::cout << "Output directory (" << j_config["build_options"]["output_dir"] << "): ";
    std::getline(std::cin, cfg_input);
    if(!cfg_input.empty()) j_config["build_options"]["output_dir"] = cfg_input;



    // Finish
    std::cout << std::endl;

    return j_config;
}

bool SetupWorkspaceDirectories(const json &buildConfig, bool doLog = true) {
    // Workspace directory
    const auto workspaceDir = (buildConfig["build_options"]["workspace_dir"]).get<std::string>();

    if(!(std::filesystem::exists(workspaceDir) && std::filesystem::is_directory(workspaceDir))) {
        std::filesystem::create_directory(workspaceDir);
        if(doLog) std::cout << "Created directory: \"" << workspaceDir << "\"." << std::endl;
    }

    // Each resource type directories
    for(auto &[resStr, resType] : g_typeStrs) {
        const std::string dir =
            workspaceDir + (buildConfig["asset_paths"][resStr + "s"]).get<std::string>();

        if(!(std::filesystem::exists(dir) && std::filesystem::is_directory(dir))) {
            std::filesystem::create_directory(dir);

            if(doLog) std::cout << "Created directory: \"" << dir << "\"." << std::endl;
        }
    }

    return true;
}

bool SetupOutputDirectories(const json &buildConfig, bool doLog = true) {
    // Output directory
    const auto outputDir = (buildConfig["build_options"]["output_dir"]).get<std::string>();

    if(!(std::filesystem::exists(outputDir) && std::filesystem::is_directory(outputDir))) {
        std::filesystem::create_directory(outputDir);
        if(doLog) std::cout << "Created directory: \"" << outputDir << "\"." << std::endl;
    }

    // Each resource type directories
    for(auto &[resStr, resType] : g_typeStrs) {
        const std::string dir =
            outputDir + (buildConfig["asset_paths"][resStr + "s"]).get<std::string>();

        if(!(std::filesystem::exists(dir) && std::filesystem::is_directory(dir))) {
            std::filesystem::create_directory(dir);
            
            if(doLog) std::cout << "Created directory: \"" << dir << "\"." << std::endl;
        }
    }

    return true;
}

bool CheckBuildConfig(const json &buildConfig) {
    // Basic metadata
    if(buildConfig.count("title") < 1)      return false;
    if(!buildConfig["title"].is_string())   return false;

    if(buildConfig.count("description") < 1)    return false;
    if(!buildConfig["description"].is_string()) return false;

    if(buildConfig.count("author") < 1)     return false;
    if(!buildConfig["author"].is_string())  return false;

    if(buildConfig.count("version") < 1)    return false;
    if(!buildConfig["version"].is_string()) return false;


    // Asset paths
    if(buildConfig.count("asset_paths") < 1)    return false;
    if(!buildConfig["asset_paths"].is_object()) return false;

    if(buildConfig["asset_paths"].count("textures") < 1)    return false;
    if(!buildConfig["asset_paths"]["textures"].is_string()) return false;

    if(buildConfig["asset_paths"].count("sprites") < 1)    return false;
    if(!buildConfig["asset_paths"]["sprites"].is_string()) return false;

    if(buildConfig["asset_paths"].count("tilesets") < 1)    return false;
    if(!buildConfig["asset_paths"]["tilesets"].is_string()) return false;

    if(buildConfig["asset_paths"].count("sounds") < 1)    return false;
    if(!buildConfig["asset_paths"]["sounds"].is_string()) return false;

    if(buildConfig["asset_paths"].count("fonts") < 1)    return false;
    if(!buildConfig["asset_paths"]["fonts"].is_string()) return false;


    // Build options
    if(buildConfig.count("build_options") < 1)    return false;
    if(!buildConfig["build_options"].is_object()) return false;

    if(buildConfig["build_options"].count("workspace_dir") < 1)     return false;
    if(!buildConfig["build_options"]["workspace_dir"].is_string())  return false;

    if(buildConfig["build_options"].count("output_dir") < 1)    return false;
    if(!buildConfig["build_options"]["output_dir"].is_string()) return false;

    if(buildConfig["build_options"].count("use_cache") < 1)     return false;
    if(!buildConfig["build_options"]["use_cache"].is_boolean()) return false;

    return true;
}

bool FixBuildConfig(json &buildConfig, const std::string &path, bool doLog = true) {
    json tempConfig;

    // Surface level keys
    for(auto &[name, value] : g_defaultBuildConfig.items()) {
        bool doRepair = buildConfig.count(name) < 1;

        if(!doRepair)
            doRepair |= (buildConfig[name].type() != g_defaultBuildConfig[name].type());

        if(doRepair) {
            bool success = true;
            try {
                buildConfig.erase(name);
                buildConfig[name] = g_defaultBuildConfig[name];
            } catch(json::exception &e) {
                success = false;
            }

        }

        tempConfig[name] = buildConfig[name];
    }

    // Asset paths
    for(auto &[name, value] : g_defaultBuildConfig["asset_paths"].items()) {
        bool doRepair = buildConfig["asset_paths"].count(name) < 1;

        if(!doRepair)
            doRepair |= (buildConfig["asset_paths"][name].type() != g_defaultBuildConfig["asset_paths"][name].type());

        if(doRepair) {
            bool success = true;
            try {
                buildConfig["asset_paths"].erase(name);
                buildConfig["asset_paths"][name] = g_defaultBuildConfig["asset_paths"][name];
            } catch(json::exception &e) {
                success = false;
            }

        }

        tempConfig["asset_paths"][name] = buildConfig["asset_paths"][name];
    }

    // Build options
    for(auto &[name, value] : g_defaultBuildConfig["build_options"].items()) {
        bool doRepair = buildConfig["build_options"].count(name) < 1;

        if(!doRepair)
            doRepair |= (buildConfig["build_options"][name].type() != g_defaultBuildConfig["build_options"][name].type());

        if(doRepair) {
            bool success = true;
            try {
                buildConfig["build_options"].erase(name);
                buildConfig["build_options"][name] = g_defaultBuildConfig["build_options"][name];
            } catch(json::exception &e) {
                success = false;
            }

        }

        tempConfig["build_options"][name] = buildConfig["build_options"][name];
    }

    // Saving
    buildConfig = tempConfig;

    std::ofstream f(GALAMAKE_CONFIG_NAME);
    f << buildConfig.dump(4);
    f.close();

    return true;
}

void rllog(int logLevel, const char *text, va_list args) {
    return;
}

int main(int argc, char **argv) {
    // Disable raylib logging
    SetTraceLogCallback(rllog);


    // Args and options
    std::vector<std::string> args(argv + 1, argv + argc);
    bool op_doDefaultConfig = false;

    if( (args.empty()) ||
        (std::find(args.begin(), args.end(), "--help") != args.end()) ||
        (std::find(args.begin(), args.end(), "-h") != args.end())
    ) {
        PrintHelp();
        return 0;
    }else if(
        (std::find(args.begin(), args.end(), "--version") != args.end()) ||
        (std::find(args.begin(), args.end(), "-v") != args.end())
    ) {
        PrintVersion();
        return 0;
    }

    if( (std::find(args.begin(), args.end(), "--default") != args.end()) ||
        (std::find(args.begin(), args.end(), "-d") != args.end())
    ) {
        op_doDefaultConfig = true;
        std::remove(args.begin(), args.end(), "--default");
        std::remove(args.begin(), args.end(), "-d");
    }


    // Config
    json j_buildConfig;
    bool configFound = std::filesystem::exists(GALAMAKE_CONFIG_NAME);
    bool validConfig = true;
    
    if(configFound) {
        std::ifstream f(GALAMAKE_CONFIG_NAME);
        try {
            j_buildConfig = json::parse(f);
            validConfig = CheckBuildConfig(j_buildConfig);
        } catch(json::exception &e) {
            validConfig = false;
        }
        f.close();
    }

    // Actions
    const std::string &actionStr = args[0];

    if(actionStr != "new") { // Unless setting up new build config, do some checks.
        if(!configFound) {
            PrintError(ToolError::NoFoundConfig);
            return 1;
        }

        if((actionStr != "repair") && !validConfig) {
            PrintError(ToolError::InvalidConfig);
            return 1;
        }
    }

    if(actionStr == "new") {
        // Build config
        json j_outConfig = op_doDefaultConfig ? g_defaultBuildConfig : GetConfigFromSetup(g_defaultBuildConfig);

        Timer timer;
        timer.Start();

        const auto outStr = j_outConfig.dump(4);
        std::cout << "Writing to '" << std::filesystem::path(GALAMAKE_CONFIG_NAME).string() << "':\n" << outStr << "\n" << std::endl;

        std::ofstream f_out(GALAMAKE_CONFIG_NAME);
        f_out << outStr;
        f_out.close();

        std::ifstream f_in(GALAMAKE_CONFIG_NAME);
        j_buildConfig = json::parse(f_in);
        f_in.close();

        std::cout << "Created new build config, '" GALAMAKE_CONFIG_NAME "'.\n" << std::endl;

        // Directory setup
        std::cout << "Setting up directories..." << std::endl;
        SetupWorkspaceDirectories(j_buildConfig);
        SetupOutputDirectories(j_buildConfig);

        // Done
        std::cout << "\nFinished in " << std::to_string(timer.Stop()) << "s." << std::endl;

        return 0;
    }else if(actionStr == "build") {
        // Guarding
        if(args.size() != 2) {
            PrintError(ToolError::InvalidArgumentCount);
            return 1;
        }

        // Getting data
        const std::string &resURI = args[1];
        const auto [resTypeStr, resName] = SplitResourceURI(resURI);

        if(g_typeStrs.count(resTypeStr) == 0) {
            PrintError(ToolError::InvalidResourceType, resTypeStr);
            return 1;
        }

        ResourceType resType = g_typeStrs[resTypeStr];
        const ResourcePathInfo resPaths = GenResourcePaths(j_buildConfig, resType, resName);

        // Exists guard
        if(!std::filesystem::exists(resPaths.inputPath)) {
            PrintError(ToolError::ResourceNotFound, resURI);
            return 1;
        }

        // Build
        std::cout << "Building " << resTypeStr << " resource: \"" << resName << "\"... ";

        Timer buildTimer;
        buildTimer.Start();

        bool success = BuildResource(ResourceInfo {
            resType,
            resName,
            resPaths
        });

        double secs = buildTimer.Stop();

        std::cout << (success ? "\e[0;32mDONE" : "\e[1;31mFAILED") << "\e[0m." << std::endl;

        std::cout << std::endl << "Finished in " << std::to_string(secs) << "s." << std::endl;

        return success;
    }else if(actionStr == "buildall") {
        // Scanning
        std::vector<std::string> resources = ScanResources(j_buildConfig);

        // Building
        Timer buildTimer;
        buildTimer.Start();

        for(auto &resURI : resources) {
            // Getting data
            const auto [resTypeStr, resName] = SplitResourceURI(resURI);

            if(g_typeStrs.count(resTypeStr) == 0) {
                PrintError(ToolError::InvalidResourceType, resTypeStr);
                return 1;
            }

            ResourceType resType = g_typeStrs[resTypeStr];
            const ResourcePathInfo resPaths = GenResourcePaths(j_buildConfig, resType, resName);

            // Exists guard
            if(!std::filesystem::exists(resPaths.inputPath)) {
                PrintError(ToolError::ResourceNotFound, resURI);
                return 1;
            }

            // Build
            std::cout << "Building " << resTypeStr << " resource: \"" << resName << "\"... ";
            const auto resInfo = ResourceInfo {
                resType,
                resName,
                resPaths
            };


            const ResourceCheckError resError = CheckResourceIntegrity(resInfo);
            if(resError != ResourceCheckError::None) {
                std::cout << ("\e[1;31m" + GetResourceCheckErrorString(resError) + "\e[0m.") << std::endl;
                continue;
            }

            const bool success = BuildResource(resInfo);
            std::cout << (success ? "\e[0;32mDONE" : "\e[1;31mFAILED") << "\e[0m." << std::endl;

            if(!success) return 1;
        }

        std::cout << std::endl << "Finished in " << std::to_string(buildTimer.Stop()) << "s." << std::endl;

        return 0;
    }else if(actionStr == "scan") {
        // Scanning
        std::vector<std::string> resources = ScanResources(j_buildConfig);

        for(auto &r : resources)
            std::cout << r << std::endl;

        return 0;
    }else if(actionStr == "report") {
        // Reporting
        std::vector<std::string> resources, missingDirs, invalidResources;

        Timer stepTimer;
        double stepTime = 0.0;
        double totalTime = 0.0;

        int totalDirs = 0;
        int totalResources = 0;

        // Directories
        stepTimer.Start();
        std::cout << "===== Directories =====" << std::endl;

        std::cout << ":: Workspace ::" << std::endl;

        const auto workspaceDir = (j_buildConfig["build_options"]["workspace_dir"]).get<std::string>();

        std::cout << "Checking for directory: \"" << workspaceDir << "\"... ";
        if(!(std::filesystem::exists(workspaceDir) && std::filesystem::is_directory(workspaceDir))) {
            missingDirs.push_back(workspaceDir);
            std::cout << "\e[1;31mNOT FOUND\e[0m." << std::endl;
        }else {
            std::cout << "\e[0;32mOK\e[0m." << std::endl;
        }
        totalDirs++;

        for(auto &[resStr, resType] : g_typeStrs) {
            const std::string dir =
                workspaceDir + (j_buildConfig["asset_paths"][resStr + "s"]).get<std::string>();

            std::cout << "Checking for directory: \"" << dir << "\"... ";

            bool success = (std::filesystem::exists(dir) && std::filesystem::is_directory(dir));
            if(!success) missingDirs.push_back(dir);

            std::cout << (success ? "\e[0;32mOK" : "\e[1;31mNOT FOUND") << "\e[0m." << std::endl;
            totalDirs++;
        }

        std::cout << std::endl;


        std::cout << ":: Output Directories ::" << std::endl;

        const auto outputDir = (j_buildConfig["build_options"]["output_dir"]).get<std::string>();

        std::cout << "Checking for directory: \"" << outputDir << "\"... ";
        if(!(std::filesystem::exists(outputDir) && std::filesystem::is_directory(outputDir))) {
            missingDirs.push_back(outputDir);
            std::cout << "\e[1;31mNOT FOUND\e[0m." << std::endl;
        }else {
            std::cout << "\e[0;32mOK\e[0m." << std::endl;
        }
        totalDirs++;

        for(auto &[resStr, resType] : g_typeStrs) {
            const std::string dir =
                outputDir + (j_buildConfig["asset_paths"][resStr + "s"]).get<std::string>();

            std::cout << "Checking for directory: \"" << dir << "\"... ";

            bool success = (std::filesystem::exists(dir) && std::filesystem::is_directory(dir));
            if(!success) missingDirs.push_back(dir);

            std::cout << (success ? "\e[0;32mOK" : "\e[1;31mNOT FOUND") << "\e[0m." << std::endl;
            totalDirs++;
        }

        stepTime = stepTimer.Stop();
        totalTime += stepTime;
        std::cout << "\nTime: " << std::to_string(stepTime) << "s\n\n" << std::endl;


        // Resources
        stepTimer.Start();
        std::cout << "===== Resources =====" << std::endl;

        resources = ScanResources(j_buildConfig, false);

        if(resources.empty())
            std::cout << "No resources." << std::endl;

        for(auto &resURI : resources) {
            const auto [resTypeStr, resName] = SplitResourceURI(resURI);

            std::cout << "Checking " << resTypeStr << " resource: \"" << resName << "\"... ";

            const ResourceType resType = g_typeStrs[resTypeStr];
            const auto resError = CheckResourceIntegrity(ResourceInfo {
                resType,
                resName,
                GenResourcePaths(j_buildConfig, resType, resName)
            });

            if(resError != ResourceCheckError::None) invalidResources.push_back(resURI);

            std::cout
                << ((resError == ResourceCheckError::None) ? "\e[0;32mOK" : ("\e[1;31m" + GetResourceCheckErrorString(resError)))
                << "\e[0m." << std::endl;

            totalResources++;
        }

        stepTime = stepTimer.Stop();
        totalTime += stepTime;
        std::cout << "\nTime: " << std::to_string(stepTime) << "s\n\n" << std::endl;


        // Stats
        std::string issues;

        if(!missingDirs.empty()) {
            issues += std::to_string(missingDirs.size()) + " missing directories:\n";

            for(auto &d : missingDirs)
                issues += "    " + d + "\n";

            issues += "\n";
        }

        if(!invalidResources.empty()) {
            issues += std::to_string(invalidResources.size()) + " invalid resources:\n";

            for(auto &r : invalidResources)
                issues += "    " + r + "\n";
        }

        if(issues.empty())
            issues = "No issues found.\n";

        std::cout
            << "===== Done =====\n"
            << issues << "\n"
            << "Scanned " << totalDirs << " directories, " << totalResources << " assets.\n"
            << "Finished in " << std::to_string(totalTime) << "s\n"
            << std::endl;

    }else if(actionStr == "repair") {
        // Fix config if needed.
        if(!validConfig) {
            std::cout << "\e[1;95mBUILD CONFIG INVALID.\e[0m\nAttempting to fix build config... ";
            
            const bool configFixSuccess = FixBuildConfig(j_buildConfig, GALAMAKE_CONFIG_NAME);

            if(configFixSuccess) {
                std::cout << "\e[0;32mDONE\e[0m\n" << std::endl;
            }else {
                std::cout << "\e[1;31mFAILED\e[0m\n" << std::endl;
                std::cout << "\e[1;95mwarning:\e[0m Using 'galamake new' to generate a new build config, or editing the existing one manually is necessary." << std::endl;
                return 1;
            }
        }

        // Reporting
        std::vector<std::string> resources, fixedDirs, fixedResources, failedResources;

        Timer stepTimer;
        double stepTime = 0.0;
        double totalTime = 0.0;

        int totalDirs = 0;
        int totalResources = 0;

        // Directories
        stepTimer.Start();
        std::cout << "===== Directories =====" << std::endl;

        std::cout << ":: Workspace ::" << std::endl;

        const auto workspaceDir = (j_buildConfig["build_options"]["workspace_dir"]).get<std::string>();

        if(!(std::filesystem::exists(workspaceDir) && std::filesystem::is_directory(workspaceDir))) {
            std::filesystem::create_directory(workspaceDir);
            std::cout << "Created directory: \"" << workspaceDir << "\"." << std::endl;
            fixedDirs.push_back(workspaceDir);
            totalDirs++;
        }

        for(auto &[resStr, resType] : g_typeStrs) {
            const std::string dir =
                workspaceDir + (j_buildConfig["asset_paths"][resStr + "s"]).get<std::string>();

            if(!(std::filesystem::exists(dir) && std::filesystem::is_directory(dir))) {
                std::filesystem::create_directory(dir);
                std::cout << "Created directory: \"" << dir << "\"." << std::endl;
                fixedDirs.push_back(dir);
            }

            totalDirs++;
        }

        std::cout << std::endl;

        std::cout << ":: Output Directories ::" << std::endl;

        const auto outputDir = (j_buildConfig["build_options"]["output_dir"]).get<std::string>();

        if(!(std::filesystem::exists(outputDir) && std::filesystem::is_directory(outputDir))) {
            std::filesystem::create_directory(outputDir);
            std::cout << "Created directory: \"" << outputDir << "\"." << std::endl;
            fixedDirs.push_back(outputDir);
            totalDirs++;
        }

        for(auto &[resStr, resType] : g_typeStrs) {
            const std::string dir =
                (j_buildConfig["build_options"]["output_dir"]).get<std::string>() +
                (j_buildConfig["asset_paths"][resStr + "s"]).get<std::string>();

            if(!(std::filesystem::exists(dir) && std::filesystem::is_directory(dir))) {
                std::filesystem::create_directory(dir);
                std::cout << "Created directory: \"" << dir << "\"." << std::endl;
                fixedDirs.push_back(dir);
            }
            
            totalDirs++;
        }

        stepTime = stepTimer.Stop();
        totalTime += stepTime;
        std::cout << "\nTime: " << std::to_string(stepTime) << "s\n\n" << std::endl;

        // Resources
        stepTimer.Start();
        std::cout << "===== Resources =====" << std::endl;

        resources = ScanResources(j_buildConfig, false);

        if(resources.empty())
            std::cout << "No resources." << std::endl;

        for(auto &resURI : resources) {
            const auto [resTypeStr, resName] = SplitResourceURI(resURI);
            const ResourceType resType = g_typeStrs[resTypeStr];

            const auto resInfo = ResourceInfo {
                resType,
                resName,
                GenResourcePaths(j_buildConfig, resType, resName)
            };

            const auto resError = CheckResourceIntegrity(resInfo);

            if(resError != ResourceCheckError::None) {
                std::cout << "Fixing " << resTypeStr << " resource: \"" << resName << "\"... ";

                bool fixSuccess = FixResource(resInfo);

                if(fixSuccess) {
                    std::cout << "\e[0;32mDONE\e[0m" << std::endl;
                    fixedResources.push_back(resURI);
                }else {
                    std::cout << "\e[1;31mFAILED\e[0m" << std::endl;
                    failedResources.push_back(resURI);
                }
            }

            totalResources++;
        }

        stepTime = stepTimer.Stop();
        totalTime += stepTime;
        std::cout << "\nTime: " << std::to_string(stepTime) << "s\n\n" << std::endl;

        // Stats
        std::string fixed;

        if(!fixedDirs.empty()) {
            fixed += "Fixed " + std::to_string(fixedDirs.size()) + " missing directories:\n";

            for(auto &d : fixedDirs)
                fixed += "    " + d + "\n";

            fixed += "\n";
        }

        if(!fixedResources.empty()) {
            fixed += "Fixed " + std::to_string(fixedResources.size()) + " invalid resources:\n";

            for(auto &r : fixedResources)
                fixed += "    " + r + "\n";
        }

        if(!failedResources.empty()) {
            fixed += "FAILED to fix " + std::to_string(totalResources - 1) + " resources:\n";

            for(auto &r : failedResources)
                fixed += "    " + r + "\n";
        }

        if(fixed.empty())
            fixed = "No issues found.\n";

        std::cout
            << "===== Done =====\n"
            << fixed << "\n"
            << "Scanned " << totalDirs << " directories, " << totalResources << " assets.\n"
            << "Finished in " << std::to_string(totalTime) << "s\n"
            << std::endl;
    } else {
        PrintError(ToolError::InvalidAction, actionStr);
        return 1;
    }

    return 0;
}