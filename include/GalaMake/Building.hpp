#pragma once

#include <GalaMake/Common.hpp>

bool BuildTextureResource (const ResourceInfo &resource);
bool BuildSpriteResource  (const ResourceInfo &resource);
bool BuildTilesetResource (const ResourceInfo &resource);
bool BuildNSliceResource  (const ResourceInfo &resource);
bool BuildSoundResource   (const ResourceInfo &resource);
bool BuildFontResource    (const ResourceInfo &resource);

bool BuildResource(const ResourceInfo &resource);