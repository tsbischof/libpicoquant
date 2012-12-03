from distutils.core import setup

setup(name="pypicoquant",
      version="0.0.1",
      author="Thomas Bischof",
      author_email="tbischof2mit.edu",
      description="A ctypes interface to Picoquant hardware libraries and types.",
      packages=["picoquant", 
                "picoquant.picoharp", 
                "picoquant.hydraharp", 
                "picoquant.timeharp"])
