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
picoharp_dir = os.path.join(lib_dir, "ph300")
picoharp_module = Extension(
     "_picoharp_comm", 
     [os.path.join("picoquant", "picoharp", "picoharp_comm.i"),
      os.path.join("picoquant", "comm_lib_common.i")],
     include_dirs=[picoharp_dir],
     swig_opts=["-I{0}".format(picoharp_dir)],
     libraries=["ph300"])
if is_64bits:
    print("PicoHarp control module must be built with 32-bit Python.")
else:
    ext_modules.append(picoharp_module)

# HydraHarp has 32-bit and 64-bit libraries.
# This is where the HH control libraries are stored.
hydraharp_dir = os.path.join(lib_dir, "hh400")
hydraharp_module = Extension(
     "_hydraharp_comm", 
     [os.path.join("picoquant", "hydraharp", "hydraharp_comm.i"),
      os.path.join("picoquant", "comm_lib_common.i")],
     include_dirs=[hydraharp_dir],
     swig_opts=["-I{0}".format(hydraharp_dir)],
     libraries=["hh400"])
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
