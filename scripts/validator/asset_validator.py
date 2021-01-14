import json
import sys
import os
import os.path
import uuid
import shutil
from pathlib import Path
from enum import Enum

from shader_validator import validate_shader
from aer_validator import validate_vkshader
from texture_validator import validate_texture
from material_validator import validate_material
from skybox_validator import validate_skybox
from pkg_validator import validate_pkg
from aer_validator import validate_aer_material
from aer_validator import validate_aer_shader


class AssetType(Enum):
    UNKNOWN = -1
    MTL = 0
    OBJ = 1
    TEXTURE = 2
    SCENE = 3
    VERT_SHADER = 4
    FRAG_SHADER = 5
    VK_SHADER = 6
    FONT = 7
    SKYBOX = 8
    PKG = 9
    AER_MAT = 10
    AER_SHADER = 11


img_extension = [
    ".jpeg",
    ".jpg",
    ".png",
    ".bmp",
    ".hdr",
    ".ktx",
    ".dds",
    ".pam",
    ".ppm",
    ".pgm"
]


def define_asset_type(filename) -> AssetType:
    global img_extension
    path = Path(filename)
    extension = path.suffix.lower()
    if extension in img_extension:
        return AssetType.TEXTURE
    if extension == '.mtl' or extension == '.mat':
        return AssetType.MTL
    if extension == '.obj' or extension == '.fbx':
        return AssetType.OBJ
    if extension == '.vert':
        return AssetType.VERT_SHADER
    if extension == '.frag':
        return AssetType.FRAG_SHADER
    if extension == '.vk':
        return AssetType.VK_SHADER
    if extension == '.ttf':
        return AssetType.FONT
    if extension == '.skybox':
        return AssetType.SKYBOX
    if extension == '.pkg_json':
        return AssetType.PKG
    if extension == '.aermat':
        return AssetType.AER_MAT
    if extension == '.aershader':
        return AssetType.AER_SHADER
    return AssetType.UNKNOWN


def validate_asset(src="", out=""):
    global data_src, data_out
    if src != "" and out != "":
        data_src = src
        data_out = out
    meta_content = {}
    asset_type = define_asset_type(data_src)
    # load meta data
    if asset_type != AssetType.UNKNOWN:
        if os.path.isfile(data_src+".meta"):
            with open(data_src+".meta", 'r') as meta_file:
                meta_content = json.load(meta_file)

        if os.path.isfile(data_out + ".meta"):
            with open(data_out + ".meta", "r") as meta_file:
                meta_content2 = json.load(meta_file)
                meta_content = {**meta_content2, **meta_content}

    # Copy file except if meta files says no
    if "copy" in meta_content:
        copy = meta_content["copy"]
    else:
        copy = True
    if copy:
        shutil.copy(data_src, data_out)

    if asset_type == AssetType.TEXTURE:
        validate_texture(data_src, data_out, meta_content)
    if asset_type == AssetType.VERT_SHADER or asset_type == AssetType.FRAG_SHADER:
        validate_shader(data_src, data_out, meta_content)
    if asset_type == AssetType.VK_SHADER:
        validate_vkshader(data_src, data_out, meta_content)
    if asset_type == AssetType.MTL:
        validate_material(data_src, data_out, meta_content)
    if asset_type == AssetType.SKYBOX:
        validate_skybox(data_src, data_out, meta_content)
    if asset_type == AssetType.PKG:
        validate_pkg(data_src, data_out, meta_content)
    if asset_type == AssetType.AER_MAT:
        validate_aer_material(data_src, data_out, meta_content)
    if asset_type == AssetType.AER_SHADER:
        validate_aer_shader(data_src, data_out, meta_content)
    # write new meta content to meta file
    if asset_type != AssetType.UNKNOWN:
        if not os.path.isfile(data_out + ".meta"):
            # We generate the meta file
            meta_content["uuid"] = str(uuid.uuid1())
        with open(data_out + ".meta", "w") as meta_file:
            json.dump(meta_content, meta_file, indent=4)


def load_env_variables():
    env_path = os.path.join(os.getenv("SRC_FOLDER"),"tmp/env.json")
    with open(env_path, 'r') as env_file:
        env_content = json.load(env_file)
        for key in env_content:
            os.environ[key] = env_content[key]


if __name__ == "__main__":
    arguments = ", ".join(sys.argv)
    print("Data validator arugments: "+arguments)
    data_src = sys.argv[1]
    data_out = sys.argv[2]
    data_out_parent = Path(data_out).parent
    if not data_out_parent.is_dir():
        data_out_parent.mkdir(parents=True, exist_ok=True)
    load_env_variables()
    validate_asset()
