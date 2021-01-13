#!/usr/bin/env python3

import platform
import subprocess
import os
from enum import Enum
from pathlib import Path

if platform.system() == 'Windows':
    vulkan_path = os.getenv("VULKAN_SDK")
    program = '{}\\Bin\\glslangValidator.exe'.format(vulkan_path)
else:
    program = 'glslangValidator'


class ShaderType(Enum):
    MISC = 0
    VERT = 1
    FRAG = 2
    TESC = 3
    TESE = 4
    GEOM = 5
    COMP = 6


type_sizes = {
    'bool': 1,
    'int': 4,
    'uint': 4,
    'float': 4,
    'double': 8,
    'vec2': 8,
    'vec3': 12,
    'vec4': 16,
    'mat2': 16,
    'mat3': 36,
    'mat4': 64,
    'sampler2D': 0
}

type_to_enum = {
    'float': 1,
    'vec2': 2,
    'vec3': 3,
    'vec4': 4,
    'int': 5,
    'ivec2': 6,
    'ivec3': 7,
    'ivec4': 8,
    'uint': 9,
    'uvec2': 10,
    'uvec3': 11,
    'uvec4': 12,
    'mat2': 13,
    'mat3': 14,
    'mat4': 15,
}

u_types = {
    'sampler2D': 1,
    'sampler3D': 2,
}

stage_flags = {
    '.vert': 1,
    '.frag': 16,
    '.geom': 8,
    '.comp': 32,
    '.tese': 4,
    '.tesc': 2,
}
def removeprefix(extension, prefix) :
    if extension.startswith(prefix):
        return extension[len(prefix):]
    else:
        return extension[:]

def validate_vkshader(data_src, data_out, meta_content):
    path = Path(data_src)
    extension = path.suffixes
    data_out = data_out.replace('.vk', '.spv')
    if extension[0] == '.vert':
        shader_type = ShaderType.VERT
    elif extension[0] == '.frag':
        shader_type = ShaderType.FRAG
    elif extension[0] == '.tesc':
        shader_type = ShaderType.TESC
    elif extension[0] == '.tese':
        shader_type = ShaderType.TESE
    elif extension[0] == '.geom':
        shader_type = ShaderType.GEOM
    elif extension[0] == '.comp':
        shader_type = ShaderType.COMP
    else:
        shader_type = ShaderType.MISC

    command = [program, "-V", "-S", removeprefix(extension[0],"."), data_src, "-o", data_out]
    status = subprocess.run(command)
    if status.returncode != 0:
        exit(1)

    meta_content["shader_type"] = shader_type.value

    attributes = []
    uniforms = []
    uniform_blocks = []
    with open(path, 'r') as shader_file:
        lines = shader_file.readlines()
        for count, line in enumerate(lines):
            colon = False
            if ';' in line:
                colon = True
            line = line.replace('\n', '')
            line = line.replace(';', '')
            line = line.replace('//', '// ')
            split_line = line.split(' ')
            if "in" in split_line:
                comment_index = len(split_line)
                if "//" in split_line:
                    comment_index = split_line.index("//")
                index = split_line.index('=')
                if index > comment_index:
                    continue
                in_variable = {
                    "name": split_line[index + 4],
                    "location": int(split_line[index + 1][0]),
                    "size": type_sizes.get(split_line[index + 3]),
                    "type": type_to_enum.get(split_line[index + 3]),
                }
                attributes.append(in_variable)
            if "uniform" in split_line and colon:
                uniform_obj = {
                    "name": split_line[5],
                    "binding": int(split_line[2][0]),
                    "offset": -1,
                    "size": -1,
                    "type": 0,
                }
                uniforms.append(uniform_obj)
            elif "uniform" in split_line:
                i = 1
                size = 0
                ub_uniforms = []
                while '}' not in lines[count]:
                    count += 1
                    line1 = lines[count].replace('\n', '')
                    line1 = line1.replace(';', '')
                    line1 = line1.replace('  ', '')
                    line1 = line1.replace('\t', '')
                    split_line1 = line1.split(' ')
                    if len(split_line1) < 2:
                        continue
                    for type_size in type_sizes:
                        if type_size in split_line1:
                            size += type_sizes.get(type_size)
                            ub_uniform = {
                                "name": split_line1[1],
                                "binding": 0,
                                "offset": size - type_sizes.get(type_size),
                                "size": type_sizes.get(type_size),
                                "type": 0,
                                "readOnly": False,
                                "writeOnly": False,
                            }
                            ub_uniforms.append(ub_uniform)
                            break

                uniform_block_obj = {
                    "name": split_line[4],
                    "binding": int(split_line[2][0]),
                    "size": size,
                    "type": 0,
                    "uniforms": ub_uniforms,
                }
                uniform_blocks.append(uniform_block_obj)
    meta_content["attributes"] = attributes
    meta_content["uniforms"] = uniforms
    meta_content["uniformBlocks"] = uniform_blocks


def validate_aer_material(data_src, data_out, meta_content):
    global validator_exe, validator_path, data_binary_path
    data_binary_path = os.getenv("DATA_BIN_FOLDER")
    validator_path = os.getenv("VALIDATOR_FOLDER")
    validator_exe = os.getenv("VALIDATE_JSON_EXE")

    status = subprocess.run([validator_exe, data_src, validator_path + "aer_material_validator.json"])
    print("Return Code: {}".format(status.returncode))
    print("validator_exe: {}".format(validator_exe))

    if status.returncode != 0:
        exit(1)

    # with open(data_src, 'r') as mat_file:
    #     mat_content = json.load(mat_file)
    #     mat_keys = mat_content.keys()
    #     new_content = {}
    #     uniforms = []
    #     for key in mat_keys:
    #         # Putting texture uuid
    #         if "map_path" in key:
    #             key_id = key.replace('path', 'id')
    #             if key_id not in mat_keys:
    #                 id = get_texture_id(os.path.join(data_binary_path, mat_content[key]))
    #                 new_content[key_id] = id
    #         # Loading shader content to material
    #         if "shader_path" in key:
    #             shader_path = os.path.join(data_binary_path, mat_content[key])
    #             with open(shader_path, 'r') as shader_content:
    #                 new_key = key.replace('path', 'content')
    #                 new_content[new_key] = shader_content.read()
    #             shader_meta_path = shader_path+".meta"
    #             with open(shader_meta_path, 'r') as shader_meta_file:
    #                 shader_meta = json.load(shader_meta_file)
    #                 if 'uniforms' in shader_meta:
    #                     for uniform_obj in shader_meta['uniforms']:
    #                         insert = True
    #                         # Avoid duplicate uniform
    #                         for u in uniforms:
    #                             if u['name'] == uniform_obj['name']:
    #                                 insert = False
    #                                 break
    #                         if insert:
    #                             uniforms.append(uniform_obj)
    #     new_content['uniforms'] = uniforms
    #     mat_content.update(new_content)
    #
    # with open(data_out, 'w') as mat_file:
    #     json.dump(mat_content, mat_file, indent=4)


def validate_aer_shader(data_src, data_out, meta_content):
    global validator_exe, validator_path, data_binary_path
    data_binary_path = os.getenv("DATA_BIN_FOLDER")
    validator_path = os.getenv("VALIDATOR_FOLDER")
    validator_exe = os.getenv("VALIDATE_JSON_EXE")

    status = subprocess.run([validator_exe, data_src, validator_path + "aer_shader_validator.json"])
    if status.returncode != 0:
        exit(1)
