import os
import sys
from distutils.core import setup, Extension

is_64bits = sys.maxsize > 2**31

lib_dir = "/usr/local/lib"
if is_64bits:
    lib_dir += "64"

ext_modules = list()
# PicoHarp only has 32-bit library, and must be built with 32-bit Python.
# This is where the PH control libraries are stored.
if sys.platform ==  "win32":
    picoharp_dirs = [os.path.join("\Program Files (x86)",
                                 "PicoQuant",
                                 "PH300-PHLibv23")]
    picoharp_libs = ["phlib"]
else:
    picoharp_dirs = [os.path.join(lib_dir, "ph300")]
    picoharp_libs = ["ph300"]
    
picoharp_module = Extension(
     "_picoharp_comm", 
     [os.path.join("picoquant", "picoharp", "picoharp_comm.i"),
      os.path.join("picoquant", "comm_lib_common.i")],
     library_dirs=picoharp_dirs,
     include_dirs=picoharp_dirs,
     swig_opts=["-I {0}".format(my_dir) for my_dir in picoharp_dirs],
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
    hydraharp_libs = ["hhlib"]
else:
    hydraharp_dirs = [os.path.join(lib_dir, "hh400")]
    hydraharp_libs = ["hh400"]
    
hydraharp_module = Extension(
     "_hydraharp_comm", 
     [os.path.join("picoquant", "hydraharp", "hydraharp_comm.i"),
      os.path.join("picoquant", "comm_lib_common.i")],
     library_dirs=hydraharp_dirs,
     include_dirs=hydraharp_dirs,
     swig_opts=["-I {0}".format(my_dir) for my_dir in hydraharp_dirs],
     libraries=hydraharp_libs)
# HydraHarp has 32-bit and 64-bit libraries
ext_modules.append(hydraharp_module)

# Currently, I do not have access to the TimeHarp contrl libraries. As such,
# they are omitted.

setup(name="pypicoquant",
      version="0.1",
      author="Thomas Bischof",
      author_email="tbischof@mit.edu",
      description="An interface to Picoquant hardware libraries "
                  "and data types.",
      ext_modules=ext_modules,
      packages=["picoquant", 
                "picoquant.picoharp", 
                "picoquant.hydraharp", 
                "picoquant.timeharp"])
