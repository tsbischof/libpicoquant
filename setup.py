import os
from distutils.core import setup, Extension

picoharp_module = Extension(
     "_picoharp_comm", 
     [os.path.join("picoquant", "picoharp", "picoharp_comm.i")],
     libraries=["ph300"])

hydraharp_module = Extension(
     "_hydraharp_comm", 
     [os.path.join("picoquant", "hydraharp", "hydraharp_comm.i")],
     libraries=["hh400"])

setup(name="pypicoquant",
      version="0.0.1",
      author="Thomas Bischof",
      author_email="tbischof@mit.edu",
      description="An interface to Picoquant hardware libraries "
                  "and data types.",
      ext_modules=[hydraharp_module, 
                   picoharp_module
                   ],
      packages=["picoquant", 
                "picoquant.picoharp", 
                "picoquant.hydraharp", 
                "picoquant.timeharp"])
