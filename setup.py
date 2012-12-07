import os
from distutils.core import setup, Extension

picoharp_module = Extension(
     "_picoharp", 
     source_files=[os.path.join("_picoharp/_picoharp.i")])

picoharp_module = Extension(
     "_hydraharp", 
     source_files=[os.path.join("_hydraharp/_hydraharp.i")])

setup(name="pypicoquant",
      version="0.0.1",
      author="Thomas Bischof",
      author_email="tbischof@mit.edu",
      description="A ctypes interface to Picoquant hardware libraries and types.",
      ext_modules=[hydraharp_module, picoharp_module],
      packages=["picoquant", 
                "picoquant.picoharp", 
                "picoquant.hydraharp", 
                "picoquant.timeharp"])
