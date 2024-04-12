# GalaMake
Build tool for GalaEngine projects.

## :warning: :rotating_light: DISCONTINUED C++ PROJECT :rotating_light: :warning:
I'm no longer using C++ for personal projects, thus this project's development has been discontinued.
Check my GitHub or [**my website**](https://colleen05.me/) for projects coming in the future.

# Usage
```
Usage: galamake <action> [args...] [options...]

Actions:
    new [args...]           Creates new 'GalaMake.json' file.
        --default or -d     Skips config setup wizard; creates default config file.

    build <resource-uri>    Builds resource file of specified resource.
    buildall                Builds project using settings in 'GalaMake.json'.
    scan                    Scans and lists each valid resource.
    report                  Scans for and lists missing direcotires and broken resources.
    repair                  Scans for and repairs broken resources and workspace structure.

Options:
    --version or -v     Display version information.
    --help    or -h     Display this help information.

Resource URI: <type>:<name>

Resource Types:
    - texture   Texture
    - sprite    Sprite
    - tileset   Tileset
    - sound     Sound
    - font      Font

GalaMake v1.0.0 - January, 2023
by Colleen (@colleen05), and GitHub contributors.

This software is distributed under the zlib license.
For more information, see the 'LICENSE' file provided, or visit:
https://choosealicense.com/licenses/zlib/
```
