#include <GalaMake/Building.hpp>

bool BuildTextureResource(const ResourceInfo &resource) {
    const std::string &sourcePath = resource.paths.inputPath;
    const std::string &outputFile = resource.paths.outputPath;

    if(!std::filesystem::exists(sourcePath)) return false;
    std::filesystem::create_directory(sourcePath + "tmp");

    // Prepare QOI texture
    Image img_texture = LoadImage(std::string(sourcePath + "texture.png").c_str());
    if(!ExportImage(img_texture, std::string(sourcePath + "tmp/texture.qoi").c_str())) return false;

    // Read resource information
    std::ifstream f(sourcePath + "resource.json");
    if(!f.good()) { f.close(); return false; }
    json j_data = json::parse(f);
    f.close();

    // Compile gres data
    xdt::Table gresTable;

    gresTable.SetString("type", "texture");

    unsigned int textureBytes = 0;
    const auto textureData = LoadFileData(std::string(sourcePath + "tmp/texture.qoi").c_str(), &textureBytes);
    gresTable.SetBytes("texture", std::vector<uint8_t>(textureData, textureData + textureBytes));

    gresTable.Save(outputFile);

    // Clean up
    UnloadFileData(textureData);
    std::filesystem::remove_all(sourcePath + "tmp");

    // Verify
    if(!std::filesystem::exists(outputFile)) return false;

    return true;
}

bool BuildSpriteResource(const ResourceInfo &resource) {
    const std::string &sourcePath = resource.paths.inputPath;
    const std::string &outputFile = resource.paths.outputPath;

    if(!std::filesystem::exists(sourcePath)) return false;
    std::filesystem::create_directory(sourcePath + "tmp");
    
    // Prepare QOI texture
    Image img_texture = LoadImage(std::string(sourcePath + "texture.png").c_str());
    if(!ExportImage(img_texture, std::string(sourcePath + "tmp/texture.qoi").c_str())) return false;

    // Read resource information
    std::ifstream f(sourcePath + "resource.json");
    if(!f.good()) { f.close(); return false; }
    json j_data = json::parse(f);
    f.close();

    // Compile gres data
    xdt::Table gresTable;

    gresTable.SetString("type", "sprite");

    gresTable.SetInt16("origin_x", j_data["origin"][0]);
    gresTable.SetInt16("origin_y", j_data["origin"][1]);
    gresTable.SetInt16("frame_count", j_data["frames"].size());

    for(auto i = 0; i < j_data["frames"].size(); i++) {
        const auto frameStr = std::to_string(i);
        gresTable.SetInt16("frame[" + frameStr + "].x", j_data["frames"][i][0]);
        gresTable.SetInt16("frame[" + frameStr + "].y", j_data["frames"][i][1]);
        gresTable.SetInt16("frame[" + frameStr + "].w", j_data["frames"][i][2]);
        gresTable.SetInt16("frame[" + frameStr + "].h", j_data["frames"][i][3]);
    }

    unsigned int textureBytes = 0;
    const auto textureData = LoadFileData(std::string(sourcePath + "tmp/texture.qoi").c_str(), &textureBytes);
    gresTable.SetBytes("texture", std::vector<uint8_t>(textureData, textureData + textureBytes));

    gresTable.Save(outputFile);

    // Clean up
    UnloadImage(img_texture);
    UnloadFileData(textureData);

    std::filesystem::remove_all(sourcePath + "tmp");

    // Verify
    if(!std::filesystem::exists(outputFile)) return false;

    return true;
}

bool BuildTilesetResource(const ResourceInfo &resource) {
    const std::string &sourcePath = resource.paths.inputPath;
    const std::string &outputFile = resource.paths.outputPath;

    if(!std::filesystem::exists(sourcePath)) return false;
    std::filesystem::create_directory(sourcePath + "tmp");

    // Prepare QOI texture
    Image img_texture = LoadImage(std::string(sourcePath + "texture.png").c_str());
    if(!ExportImage(img_texture, std::string(sourcePath + "tmp/texture.qoi").c_str())) return false;

    // Read resource information
    std::ifstream f(sourcePath + "resource.json");
    if(!f.good()) { f.close(); return false; }
    json j_data = json::parse(f);
    f.close();

    // Compile gres data
    xdt::Table gresTable;

    gresTable.SetString("type", "tileset");

    gresTable.SetInt16("tile_size", j_data["tile_size"]);

    const auto flagData = j_data["flags"].get<std::vector<uint16_t>>();
    auto flagBytes = std::vector<uint8_t>(flagData.size() * 2, 0x00);

    for(auto i = 0; i < flagData.size(); i++) { // Convert 16-bit flags into bytes.
        flagBytes[i*2 + 0] = (flagData[i] & 0x00FF) >> 0;
        flagBytes[i*2 + 1] = (flagData[i] & 0xFF00) >> 8;
    }

    gresTable.SetBytes("flags", flagBytes);

    unsigned int textureBytes = 0;
    const auto textureData = LoadFileData(std::string(sourcePath + "tmp/texture.qoi").c_str(), &textureBytes);
    gresTable.SetBytes("texture", std::vector<uint8_t>(textureData, textureData + textureBytes));

    gresTable.Save(outputFile);

    // Clean up
    UnloadFileData(textureData);
    std::filesystem::remove_all(sourcePath + "tmp");

    // Verify
    if(!std::filesystem::exists(outputFile)) return false;

    return true;
}

bool BuildSoundResource(const ResourceInfo &resource) {
    const std::string &sourcePath = resource.paths.inputPath;
    const std::string &outputFile = resource.paths.outputPath;

    if(!std::filesystem::exists(sourcePath)) return false;

    // Determine audio type
    enum class AudioType {
        Unknown,
        Wave,
        Ogg
    } audioType = AudioType::Unknown;

    std::string audioPath = "";

    if(std::filesystem::exists(sourcePath + "audio.ogg")) {
        audioPath = sourcePath + "audio.ogg";
        audioType = AudioType::Ogg;
    }else if(std::filesystem::exists(sourcePath + "audio.wav")){
        audioPath = sourcePath + "audio.wav";
        audioType = AudioType::Wave;
    }

    if(audioType == AudioType::Unknown) return false;

    // Verify audio
    bool audioLoadSuccess = true;

    Wave wav_audio = LoadWave(audioPath.c_str());
    if( (wav_audio.channels == 0) ||
        (wav_audio.data == NULL) ||
        (wav_audio.frameCount == 0) ||
        (wav_audio.sampleRate == 0) ||
        (wav_audio.sampleSize == 0)
    ) audioLoadSuccess = false;
    UnloadWave(wav_audio);

    if(!audioLoadSuccess) return false;

    // Read resource information
    std::ifstream f(sourcePath + "resource.json");
    if(!f.good()) { f.close(); return false; }
    json j_data = json::parse(f);
    f.close();

    // Compile gres data
    xdt::Table gresTable;

    std::map<AudioType, std::string> typeNames = {
        {AudioType::Ogg, "vorbis"},
        {AudioType::Wave, "wave"},
        {AudioType::Unknown, "unknown"}
    };

    gresTable.SetString("type", "sound");
    gresTable.SetString("encoding", typeNames[audioType]);

    unsigned int audioBytes = 0;
    auto audioData = LoadFileData(std::string(audioPath).c_str(), &audioBytes);
    gresTable.SetBytes("audio", std::vector<uint8_t>(audioData, audioData + audioBytes));

    gresTable.Save(outputFile);

    // Clean up
    UnloadFileData(audioData);

    // Verify
    if(!std::filesystem::exists(outputFile)) return false;

    return true;
}

bool BuildFontResource(const ResourceInfo &resource) {
    const std::string &sourcePath = resource.paths.inputPath;
    const std::string &outputFile = resource.paths.outputPath;

    if(!std::filesystem::exists(sourcePath)) return false;

    // Read resource information
    std::ifstream f(sourcePath + "resource.json");
    if(!f.good()) { f.close(); return false; }
    json j_data = json::parse(f);
    f.close();

    // Compile gres data
    xdt::Table gresTable;

    return true;
}

bool BuildResource(const ResourceInfo &resource) {
    switch(resource.type) {
        case ResourceType::Texture: return BuildTextureResource(resource); break;
        case ResourceType::Sprite:  return BuildSpriteResource(resource); break;
        case ResourceType::Tileset: return BuildTilesetResource(resource); break;
        case ResourceType::Sound:   return BuildSoundResource(resource); break;
        case ResourceType::Font:    return BuildFontResource(resource); break;
        default:
            return false;
            break;
    }

    return false;
}