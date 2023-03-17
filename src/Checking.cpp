#include <GalaMake/Checking.hpp>

std::string GetResourceCheckErrorString(const ResourceCheckError &error) {
    switch(error) {
        case ResourceCheckError::None: return "NONE";
        case ResourceCheckError::MissingConfig:     return "MISSING CONFIG";
        case ResourceCheckError::MissingContent:    return "MISSING CONTENT";
        case ResourceCheckError::InvalidConfig:     return "INVALID CONFIG";
        case ResourceCheckError::InvalidContent:    return "INVALID CONTENT";
    }

    return "UNKNOWN";
}

ResourceCheckError CheckTextureResourceIntegrity(const ResourceInfo &resource) {
    if(!std::filesystem::exists(resource.paths.inputPath + "resource.json"))
        return ResourceCheckError::MissingConfig;

    if(!std::filesystem::exists(resource.paths.inputPath + "texture.png"))
        return ResourceCheckError::MissingContent;

    return ResourceCheckError::None;
}

ResourceCheckError CheckSpriteResourceIntegrity(const ResourceInfo &resource) {
    if(!std::filesystem::exists(resource.paths.inputPath + "resource.json"))
        return ResourceCheckError::MissingConfig;

    if(!std::filesystem::exists(resource.paths.inputPath + "texture.png"))
        return ResourceCheckError::MissingContent;

    // Config checking
    json config;
    std::ifstream f(resource.paths.inputPath + "resource.json");
    try {
        config = json::parse(f);
    } catch(json::exception &e) {
        return ResourceCheckError::InvalidConfig;
    }
    f.close();

    if((config.count("origin") < 1) || (config.count("frames") < 1))
        return ResourceCheckError::InvalidConfig;

    if(config["origin"].size() < 2)
        return ResourceCheckError::InvalidConfig;

    if( (!config["origin"][0].is_number()) ||
        (!config["origin"][1].is_number())
    ) return ResourceCheckError::InvalidConfig;

    if(config["frames"].size() < 1)
        return ResourceCheckError::InvalidConfig;

    for(auto &f : config["frames"]) {
        if(!f.is_array())
            return ResourceCheckError::InvalidConfig;
        
        if(f.size() < 4)
            return ResourceCheckError::InvalidConfig;

        for(auto &e : f) {
            if(!e.is_number())
                return ResourceCheckError::InvalidConfig;
        }
    }

    return ResourceCheckError::None;
}

ResourceCheckError CheckTilesetResourceIntegrity(const ResourceInfo &resource) {
    if(!std::filesystem::exists(resource.paths.inputPath + "resource.json"))
        return ResourceCheckError::MissingConfig;

    if(!std::filesystem::exists(resource.paths.inputPath + "texture.png"))
        return ResourceCheckError::MissingContent;

    json config;
    std::ifstream f(resource.paths.inputPath + "resource.json");
    try {
        config = json::parse(f);
    } catch(json::exception &e) {
        return ResourceCheckError::InvalidConfig;
    }
    f.close();

    if(config.count("tile_size") < 1)
        return ResourceCheckError::InvalidConfig;

    if(!config["tile_size"].is_number())
        return ResourceCheckError::InvalidConfig;

    if(config.count("flags") < 1)
        return ResourceCheckError::InvalidConfig;

    if(!config["flags"].is_array())
        return ResourceCheckError::InvalidConfig;

    for(auto &f : config["flags"]) {
        if(!f.is_number())
            return ResourceCheckError::InvalidConfig;
    }

    return ResourceCheckError::None;
}

ResourceCheckError CheckNSliceResourceIntegrity(const ResourceInfo &resource) {
    if(!std::filesystem::exists(resource.paths.inputPath + "resource.json"))
        return ResourceCheckError::MissingConfig;

    if(!std::filesystem::exists(resource.paths.inputPath + "texture.png"))
        return ResourceCheckError::MissingContent;

    // Config checking
    json config;
    std::ifstream f(resource.paths.inputPath + "resource.json");
    try {
        config = json::parse(f);
    } catch(json::exception &e) {
        return ResourceCheckError::InvalidConfig;
    }
    f.close();

    if(config.count("centre_slice") < 1)    return ResourceCheckError::InvalidConfig;
    if(config["centre_slice"].size() < 4)   return ResourceCheckError::InvalidConfig;

    if(config.count("stretch_slices") < 1)  return ResourceCheckError::InvalidConfig;
    if(config["stretch_slices"].size() < 5) return ResourceCheckError::InvalidConfig;

    return ResourceCheckError::None;
}

ResourceCheckError CheckSoundResourceIntegrity(const ResourceInfo &resource) {
    if(!std::filesystem::exists(resource.paths.inputPath + "resource.json"))
        return ResourceCheckError::MissingConfig;

    if(!(
        std::filesystem::exists(resource.paths.inputPath + "audio.ogg") ||
        std::filesystem::exists(resource.paths.inputPath + "audio.wav")
    )) return ResourceCheckError::MissingContent;

    return ResourceCheckError::None;
}

ResourceCheckError CheckFontResourceIntegrity(const ResourceInfo &resource) {
    if(!std::filesystem::exists(resource.paths.inputPath + "resource.json"))
        return ResourceCheckError::MissingConfig;

    if(!std::filesystem::exists(resource.paths.inputPath + "font.ttf"))
        return ResourceCheckError::MissingContent;

    return ResourceCheckError::None;
}

ResourceCheckError CheckResourceIntegrity(const ResourceInfo &resource) {
    switch(resource.type) {
        case ResourceType::Texture: return CheckTextureResourceIntegrity(resource); break;
        case ResourceType::Sprite:  return CheckSpriteResourceIntegrity(resource); break;
        case ResourceType::Tileset: return CheckTilesetResourceIntegrity(resource); break;
        case ResourceType::NSlice:  return CheckNSliceResourceIntegrity(resource); break;
        case ResourceType::Sound:   return CheckSoundResourceIntegrity(resource); break;
        case ResourceType::Font:    return CheckFontResourceIntegrity(resource); break;
        default:
            return ResourceCheckError::None;
            break;
    }

    return ResourceCheckError::None;
}