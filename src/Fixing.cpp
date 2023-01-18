#include <GalaMake/Fixing.hpp>

bool FixTextureResource(const ResourceInfo &resource) {
    if(!std::filesystem::exists(resource.paths.inputPath + "texture.png"))
        return false;

    if(!std::filesystem::exists(resource.paths.inputPath + "resource.json")) {
        std::ofstream f(resource.paths.inputPath + "resource.json");
        f << "{\n}";
        f.close();

        if(!std::filesystem::exists(resource.paths.inputPath + "resource.json"))
            return false;
    }

    return true;
}

bool FixSpriteResource(const ResourceInfo &resource) {
    if(!std::filesystem::exists(resource.paths.inputPath + "texture.png"))
        return false;

    Image img = LoadImage((resource.paths.inputPath + "texture.png").c_str());
    const int imageWidth = img.width;
    const int imageHeight = img.height;
    UnloadImage(img);

    if(!std::filesystem::exists(resource.paths.inputPath + "resource.json")) {
        json config = {
            {"origin", {0, 0}},
            {"frames", {
                {0, 0, imageWidth, imageHeight}
            }}
        };

        std::ofstream f(resource.paths.inputPath + "resource.json");
        f << config.dump(4);
        f.close();

        if(!std::filesystem::exists(resource.paths.inputPath + "resource.json"))
            return false;
    }

    return true;
}

bool FixTilesetResource(const ResourceInfo &resource) {
    if(!std::filesystem::exists(resource.paths.inputPath + "texture.png"))
        return false;

    if(!std::filesystem::exists(resource.paths.inputPath + "resource.json")) {
        json config = {
            {"tile_size", 16},
            {"flags", {0}}
        };

        std::ofstream f(resource.paths.inputPath + "resource.json");
        f << config.dump(4);
        f.close();


        if(!std::filesystem::exists(resource.paths.inputPath + "resource.json"))
            return false;
    }

    return true;
}

bool FixSoundResource(const ResourceInfo &resource) {
    if(!(
        std::filesystem::exists(resource.paths.inputPath + "audio.ogg") ||
        std::filesystem::exists(resource.paths.inputPath + "audio.wav")
    )) return false;

    if(!std::filesystem::exists(resource.paths.inputPath + "resource.json")) {
        std::ofstream f(resource.paths.inputPath + "resource.json");
        f << "{\n}";
        f.close();

        if(!std::filesystem::exists(resource.paths.inputPath + "resource.json"))
            return false;
    }

    return true;
}

bool FixFontResource(const ResourceInfo &resource) {
    if(!std::filesystem::exists(resource.paths.inputPath + "font.ttf"))
        return false;

    if(!std::filesystem::exists(resource.paths.inputPath + "resource.json")) {
        std::ofstream f(resource.paths.inputPath + "resource.json");
        f << "{\n}";
        f.close();

        if(!std::filesystem::exists(resource.paths.inputPath + "resource.json"))
            return false;
    }

    return true;
}

bool FixResource(const ResourceInfo &resource) {
    switch(resource.type) {
        case ResourceType::Texture: return FixTextureResource(resource); break;
        case ResourceType::Sprite:  return FixSpriteResource(resource); break;
        case ResourceType::Tileset: return FixTilesetResource(resource); break;
        case ResourceType::Sound:   return FixSoundResource(resource); break;
        case ResourceType::Font:    return FixFontResource(resource); break;
        default:
            return false;
            break;
    }

    return false;
}