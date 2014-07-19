import os
import sys
from distutils.core import setup, Extension

# There are two distinct module types to build:
# 1. File encoding/decoding
# 2. Instrument control.
# 
# The file decoders are portable and thus always built.
#
# The instrument control modules require the existence of the hardware library
# and thus we must search for those before building. 

ext_modules = list()

# Prepare the file encoders.
libpicoquant_files = [os.path.join("picoquant", "files.i")]

for root, dirs, files in os.walk("src"):
    for filename in files:
        if filename.endswith(".c") and "main" not in filename:
            libpicoquant_files.append(os.path.join(root, filename))

files_module = Extension(
    "_files",
    libpicoquant_files,
    define_macros=[("VERSION", "0.1"),
                   ("_FILE_OFFSET_BITS", "64")],
    library_dirs=[],
    include_dirs=["src"],
    swig_opts=["-Isrc"],
    libraries=[])

ext_modules.append(files_module)

# Locate the control libraries, and build all that can be found.
is_64bits = sys.maxsize > 2**31
unix_lib_dir = os.path.join("/", "usr", "local", "lib")
if is_64bits:
    unix_lib_dir += "64"

if sys.platform ==  "win32":
    picoharp_dirs = [os.path.join("/",
                                  "Program Files (x86)",
                                  "PicoQuant",
                                  "PH300-PHLibv23")]
    picoharp_libs = ["phlib"]
else:
    picoharp_dirs = [os.path.join(unix_lib_dir, "ph300")]
    picoharp_libs = ["ph300"]
    
picoharp_module = Extension(
     "_picoharp_comm", 
     [os.path.join("picoquant", "picoharp", "picoharp_comm.i"),
      os.path.join("picoquant", "comm_lib_common.i")],
     library_dirs=picoharp_dirs,
     include_dirs=picoharp_dirs,
     swig_opts=["-I{0}".format(my_dir) for my_dir in picoharp_dirs],
     libraries=picoharp_libs)

if is_64bits:
    print("PicoHarp control module must be built with 32-bit Python.")
else:
    ext_modules.append(picoharp_module)

# HydraHarp has 32-bit and 64-bit libraries.
# This is where the HH control libraries are stored.
if sys.platform == "win32":
    hydraharp_dirs = [os.path.join("\Program Files",
                                   "PicoQuant",
                                   "HydraHarp-HHLibv20")]
    if is_64bits:
        hydraharp_libs = ["hhlib64"]
    else:
        hydraharp_libs = ["hhlib"]
else:
    hydraharp_dirs = [os.path.join(unix_lib_dir, "hh400")]
    hydraharp_libs = ["hh400"]
    
hydraharp_module = Extension(
     "_hydraharp_comm", 
     [os.path.join("picoquant", "hydraharp", "hydraharp_comm.i"),
      os.path.join("picoquant", "comm_lib_common.i")],
     library_dirs=hydraharp_dirs,
     include_dirs=hydraharp_dirs,
     swig_opts=["-I{0}".format(my_dir) for my_dir in hydraharp_dirs],
     libraries=hydraharp_libs)

# HydraHarp has 32-bit and 64-bit libraries
ext_modules.append(hydraharp_module)


# timeharp
if sys.platform == "win32":
    timeharp_dirs = [os.path.join("\Program Files (x86)",
                                  "PicoQuant",
                                  "TH200Libv50")]
    timeharp_libs = ["Thlib"]
else:
    timeharp_dirs = [os.path.join(unix_lib_dir, "th200")]
    timeharp_libs = ["th200"]
    
timeharp_module = Extension(
     "_timeharp_comm", 
     [os.path.join("picoquant", "timeharp", "timeharp_comm.i"),
      os.path.join("picoquant", "comm_lib_common.i")],
     library_dirs=timeharp_dirs,
     include_dirs=timeharp_dirs,
     swig_opts=["-I{0}".format(my_dir) for my_dir in timeharp_dirs],
     libraries=timeharp_libs)

if is_64bits:
    print("Timeharp requires 32-bit system")
else:
    ext_modules.append(timeharp_module)


setup(name="pypicoquant",
      version="0.1",
      author="Thomas Bischof",
      author_email="tbischof@mit.edu",
      description="An interface to Picoquant hardware libraries "
                  "and data types.",
      long_description=open("README").read(),
      ext_modules=ext_modules,
      packages=["picoquant", 
                "picoquant.picoharp", 
                "picoquant.hydraharp", 
                "picoquant.timeharp"])
