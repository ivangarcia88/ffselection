import sys
import distutils.sysconfig
from distutils.core import setup
from distutils.extension import Extension

cfg_vars = distutils.sysconfig.get_config_vars()
for key, value in cfg_vars.items():
    if type(value) == str:
        cfg_vars[key] = value.replace("-Wstrict-prototypes", "")

if(int(sys.version[0])==2):
    setup(name="pymictools",
        ext_modules=[
          Extension(
              "mictools",
              ["pymictools.cpp", "approxmaxmi.cpp", "pearson.cpp",
               "quicksort.cpp", "samic.cpp", "grid.cpp", "mictools.cpp",
               "parallelmic.cpp"],
              libraries=["boost_python"])
        ])
elif(int(sys.version[0])==3):
    setup(name="pymictools",
        ext_modules=[
          Extension(
              "mictools",
              ["pymictools.cpp", "approxmaxmi.cpp", "pearson.cpp",
               "quicksort.cpp", "samic.cpp", "grid.cpp", "mictools.cpp",
               "parallelmic.cpp"],
              libraries=["boost_python3"])
        ])
else:
  print(sys.version, "not supported")
