import os
from distutils.core import setup, Extension

picoquant_module = Extension("_picoquant", 
         sources=list(map(lambda x: os.path.join("_picoquant", "picoharp", x),
                     ["errorcodes.i", "phdefin.i", "phlib.i"])) +
                 list(map(lambda x: os.path.join("_picoquant", "hydraharp", x),
                     ["errorcodes.i", "hhdefin.i", "hhlib.i"])))

setup(name="pypicoquant",
      version="0.0.1",
      author="Thomas Bischof",
      author_email="tbischof@mit.edu",
      description="A ctypes interface to Picoquant hardware libraries and types.",
      ext_modules=[picoquant_module],
      packages=["picoquant", 
                "picoquant.picoharp", 
                "picoquant.hydraharp", 
                "picoquant.timeharp"])
