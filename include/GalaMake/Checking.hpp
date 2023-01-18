#pragma once

#include <GalaMake/Common.hpp>

enum class ResourceCheckError {
    None,
    MissingConfig,
    MissingContent,
    InvalidConfig,
    InvalidContent
};

std::string GetResourceCheckErrorString(const ResourceCheckError &error);

ResourceCheckError CheckTextureResourceIntegrity (const ResourceInfo &resource);
ResourceCheckError CheckSpriteResourceIntegrity  (const ResourceInfo &resource);
ResourceCheckError CheckTilesetResourceIntegrity (const ResourceInfo &resource);
ResourceCheckError CheckSoundResourceIntegrity   (const ResourceInfo &resource);
ResourceCheckError CheckFontResourceIntegrity    (const ResourceInfo &resource);

ResourceCheckError CheckResourceIntegrity(const ResourceInfo &resource);