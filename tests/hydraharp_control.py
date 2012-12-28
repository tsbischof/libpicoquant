import sys
sys.path.append("..")
from picoquant.hydraharp import HydraHarp

hh = HydraHarp()

print(hh.modes())
print(hh.flags())

