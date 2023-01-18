#pragma once

#include <GalaMake/Common.hpp>

ResourcePathInfo GetTexturesDirectories (const json &buildConfig);
ResourcePathInfo GetSpritesDirectories  (const json &buildConfig);
ResourcePathInfo GetTilesetsDirectories (const json &buildConfig);
ResourcePathInfo GetSoundsDirectories   (const json &buildConfig);
ResourcePathInfo GetFontsDirectories    (const json &buildConfig);

ResourcePathInfo GenResourcePaths(const json &buildConfig, ResourceType type, const std::string &resourceName);