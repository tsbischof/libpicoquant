import sys
sys.path.append("..")
from picoquant.timeharp import TimeHarp

th = TimeHarp()

print(th.library_version())
print(th.modes())
print(th.flags())

