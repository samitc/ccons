#!/usr/bin/python
import os
import sys
import subprocess
import shutil

if not os.path.exists("Dependency"):
    os.makedirs("Dependency")
    os.makedirs(os.path.join("Dependency", "gtest"))
elif not os.path.exists(os.path.join("Dependency", "gtest")):
    os.makedirs(os.path.join("Dependency", "gtest"))
total = len(sys.argv)
platform = sys.argv[1]
configuration = sys.argv[2]
cmakePath = shutil.which("cmake")
cmakeCreateArgs = [cmakePath,"-Dgtest_force_shared_crt=ON",
                   "." + os.sep + "DependencyModules" + os.sep + "googletest" + os.sep + "googletest" + os.sep + "CMakeLists.txt"]
if total == 4:
    generetor = sys.argv[3]
    cmakeCreateArgs.append("-G")
    cmakeCreateArgs.append(generetor)
cmakeBuildArgs = [cmakePath, "--build",
                  "." + os.sep + "DependencyModules" + os.sep + "googletest" + os.sep + "googletest", "--config",
                  configuration]
if not (platform == "x86" or platform == "x64"):
    exit(1)
subprocess.call(cmakeCreateArgs)
subprocess.call(cmakeBuildArgs)
if os.path.exists(
                                                                                        "." + os.sep + "DependencyModules" + os.sep + "googletest" + os.sep + "googletest" + os.sep + configuration + os.sep + "gtest.lib"):
    gtestLibName = "gtest.lib"
    gtestLibPath = "." + os.sep + "DependencyModules" + os.sep + "googletest" + os.sep + "googletest" + os.sep + configuration + os.sep + "gtest.lib"
elif os.path.exists(
                                                                        "." + os.sep + "DependencyModules" + os.sep + "googletest" + os.sep + "googletest" + os.sep + "libgtest.a"):
    gtestLibName = "libgtest.a"
    gtestLibPath = "." + os.sep + "DependencyModules" + os.sep + "googletest" + os.sep + "googletest" + os.sep + "libgtest.a"
os.rename(gtestLibPath, "." + os.sep + "Dependency" + os.sep + gtestLibName)
if not os.path.exists("." + os.sep + "Dependency" + os.sep + "gtest" + os.sep + "include"):
    shutil.copytree("." + os.sep + "DependencyModules" + os.sep + "googletest" + os.sep + "googletest" + os.sep + "include",
                "." + os.sep + "Dependency" + os.sep + "gtest" + os.sep + "include")
