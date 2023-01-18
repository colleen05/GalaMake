#pragma once

#include <iostream>

#include <nlohmann/json.hpp>
#include <libxdt.hpp>
#include <raylib.h>

#define GALAMAKE_CONFIG_NAME "GalaMake.json"

using json = nlohmann::ordered_json;

enum class ToolError {
    Unknown,
    InvalidAction,
    InvalidArgumentCount,
    NoFoundConfig,
    InvalidConfig,
    ResourceNotFound,
    InvalidResourceType,
    InvalidResourceData
};

enum class ResourceType {
    Texture,
    Sprite,
    Tileset,
    Sound,
    Font
};

struct ResourcePathInfo {
    std::string inputPath;
    std::string outputPath;
};

struct ResourceInfo {
    ResourceType type;
    std::string name;
    ResourcePathInfo paths;
};

static std::map<std::string, ResourceType> g_typeStrs = {
    {"texture", ResourceType::Texture},
    {"sprite",  ResourceType::Sprite},
    {"tileset", ResourceType::Tileset},
    {"sound",   ResourceType::Sound},
    {"font",    ResourceType::Font}
};

static json g_defaultBuildConfig = {
    {"title", "My Game"},
    {"description", ""},
    {"author", ""},
    {"version", "1.0.0"},

    {"asset_paths", {
        {"textures", "textures/"},
        {"sprites", "sprites/"},
        {"tilesets", "tilesets/"},
        {"sounds", "sounds/"},
        {"fonts", "fonts/"}
    }},

    {"build_options", {
        {"workspace_dir", "./workspace/"},
        {"output_dir", "./resources/"},
        {"use_cache", true}
    }}
};