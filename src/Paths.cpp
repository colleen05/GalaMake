#include <GalaMake/Paths.hpp>

ResourcePathInfo GetTexturesDirectories(const json &buildConfig) {
    const std::string inputDirectory =
        (buildConfig["build_options"]["workspace_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["textures"]).get<std::string>();

    const std::string outputDirectory =
        (buildConfig["build_options"]["output_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["textures"]).get<std::string>();

    return {inputDirectory, outputDirectory};
}

ResourcePathInfo GetSpritesDirectories(const json &buildConfig) {
    const std::string inputDirectory =
        (buildConfig["build_options"]["workspace_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["sprites"]).get<std::string>();

    const std::string outputDirectory =
        (buildConfig["build_options"]["output_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["sprites"]).get<std::string>();

    return {inputDirectory, outputDirectory};
}

ResourcePathInfo GetTilesetsDirectories(const json &buildConfig) {
    const std::string inputDirectory =
        (buildConfig["build_options"]["workspace_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["tilesets"]).get<std::string>();

    const std::string outputDirectory =
        (buildConfig["build_options"]["output_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["tilesets"]).get<std::string>();

    return {inputDirectory, outputDirectory};
}

ResourcePathInfo GetNSlicesDirectories(const json &buildConfig) {
    const std::string inputDirectory =
        (buildConfig["build_options"]["workspace_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["nslices"]).get<std::string>();

    const std::string outputDirectory =
        (buildConfig["build_options"]["output_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["nslices"]).get<std::string>();

    return {inputDirectory, outputDirectory};
}

ResourcePathInfo GetSoundsDirectories(const json &buildConfig) {
    const std::string inputDirectory =
        (buildConfig["build_options"]["workspace_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["sounds"]).get<std::string>();

    const std::string outputDirectory =
        (buildConfig["build_options"]["output_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["sounds"]).get<std::string>();

    return {inputDirectory, outputDirectory};
}

ResourcePathInfo GetFontsDirectories(const json &buildConfig) {
    const std::string inputDirectory =
        (buildConfig["build_options"]["workspace_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["fonts"]).get<std::string>();

    const std::string outputDirectory =
        (buildConfig["build_options"]["output_dir"]).get<std::string>() +
        (buildConfig["asset_paths"]["fonts"]).get<std::string>();

    return {inputDirectory, outputDirectory};
}

ResourcePathInfo GenResourcePaths(const json &buildConfig, ResourceType type, const std::string &resourceName) {
    ResourcePathInfo resDirs;

    switch(type) {
        case ResourceType::Texture: resDirs = GetTexturesDirectories(buildConfig); break;
        case ResourceType::Sprite:  resDirs = GetSpritesDirectories(buildConfig); break;
        case ResourceType::Tileset: resDirs = GetTilesetsDirectories(buildConfig); break;
        case ResourceType::NSlice:  resDirs = GetNSlicesDirectories(buildConfig); break;
        case ResourceType::Sound:   resDirs = GetSoundsDirectories(buildConfig); break;
        case ResourceType::Font:    resDirs = GetFontsDirectories(buildConfig); break;
        default:
            resDirs = {"", ""};
            break;
    }

    return {
        resDirs.inputPath + resourceName + "/",
        resDirs.outputPath + resourceName + ".gres"
    };
}