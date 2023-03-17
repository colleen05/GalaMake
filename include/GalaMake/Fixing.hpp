#pragma once

#include <GalaMake/Common.hpp>

bool FixTextureResource (const ResourceInfo &resource);
bool FixSpriteResource  (const ResourceInfo &resource);
bool FixTilesetResource (const ResourceInfo &resource);
bool FixNSliceResource  (const ResourceInfo &resource);
bool FixSoundResource   (const ResourceInfo &resource);
bool FixFontResource    (const ResourceInfo &resource);

bool FixResource(const ResourceInfo &resource);