import os
import sys
from distutils.core import setup, Extension

is_64bits = sys.maxsize > 2**31

ext_modules = list()
# PicoHarp only has 32-bit library, and must be built with 32-bit Python.
picoharp_module = Extension(
     "_picoharp_comm", 
     [os.path.join("picoquant", "picoharp", "picoharp_comm.i")],
     include_dirs=["/usr/local/lib/ph300"],
     swig_opts=["-I /usr/local/lib/ph300",
                "-I./picoquant"],
     libraries=["ph300"])
if is_64bits:
    print("PicoHarp must be built with 32-bit Python.")
else:
    ext_modules.append(picoharp_module)

# HydraHarp has 32-bit and 64-bit libraries.
hydraharp_module = Extension(
     "_hydraharp_comm", 
     [os.path.join("picoquant", "hydraharp", "hydraharp_comm.i")],
     include_dirs=["/usr/local/lib64/hh400"],
     swig_opts=["-I/usr/local/lib/hh400",
                "-I./picoquant"],
     libraries=["hh400"])
ext_modules.append(hydraharp_module)


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
