#!/usr/bin/env python3
import platform
import subprocess
import os
from os import environ
from enum import Enum
from pathlib import Path


class ShaderType(Enum):
    MISC = 0
    VERT = 1
    FRAG = 2
    TESC = 3
    TESE = 4
    GEOM = 5
    COMP = 6


class UniformObject:
    def __init__(self, name, type):
        self.name = name
        self.type = type


def validate_shader(data_src, data_out, meta_content):
    global glslang_exe
    glslang_exe = environ.get("GLSLANG_VALIDATOR_EXE")
    if glslang_exe is None:
        sys.stderr.write("Could not find glslangValidator executable\n")
        exit(1)

    base_file = data_src.replace('.vk', '')
    path = Path(base_file)
    stage = path.suffix.replace('.', '')
    data_out = base_file + '.spv'
    status = subprocess.run([glslang_exe, '-V', '-S', stage, data_src, '-o', data_out])
    if status.returncode != 0:
        exit(1)

