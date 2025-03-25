# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/quazip-master/lib/bzip2")
  file(MAKE_DIRECTORY "D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/quazip-master/lib/bzip2")
endif()
file(MAKE_DIRECTORY
  "D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/quazip-master/bulid_ming64/_deps/bzip2-build"
  "D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/quazip-master/bulid_ming64/_deps/bzip2-subbuild/bzip2-populate-prefix"
  "D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/quazip-master/bulid_ming64/_deps/bzip2-subbuild/bzip2-populate-prefix/tmp"
  "D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/quazip-master/bulid_ming64/_deps/bzip2-subbuild/bzip2-populate-prefix/src/bzip2-populate-stamp"
  "D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/quazip-master/bulid_ming64/_deps/bzip2-subbuild/bzip2-populate-prefix/src"
  "D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/quazip-master/bulid_ming64/_deps/bzip2-subbuild/bzip2-populate-prefix/src/bzip2-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/quazip-master/bulid_ming64/_deps/bzip2-subbuild/bzip2-populate-prefix/src/bzip2-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/quazip-master/bulid_ming64/_deps/bzip2-subbuild/bzip2-populate-prefix/src/bzip2-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
