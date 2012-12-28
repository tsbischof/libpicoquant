import os
import sys
sys.path.append("..")

from picoquant import Picoquant

suffixes = ["t2", "t3", "t3r", "hd", "hc"]

sample_data = list()

for root, dirs, files in os.walk(os.path.join("..", "sample_data")):
    for filename in files:
        if any(map(lambda x: filename.endswith(x), suffixes)):
            sample_data.append(os.path.join(root, filename))

for filename in sample_data:
    print(filename)

    p = Picoquant(filename)
    # mode
    print(p.mode())

    # header
    print(p.header())
    
    # resolution
    print(p.resolution())
