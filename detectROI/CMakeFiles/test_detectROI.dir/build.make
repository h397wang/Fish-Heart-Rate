# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI

# Include any dependencies generated for this target.
include CMakeFiles/test_detectROI.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_detectROI.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_detectROI.dir/flags.make

CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o: CMakeFiles/test_detectROI.dir/flags.make
CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o: test_detectROI.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o -c /Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI/test_detectROI.cpp

CMakeFiles/test_detectROI.dir/test_detectROI.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_detectROI.dir/test_detectROI.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI/test_detectROI.cpp > CMakeFiles/test_detectROI.dir/test_detectROI.cpp.i

CMakeFiles/test_detectROI.dir/test_detectROI.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_detectROI.dir/test_detectROI.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI/test_detectROI.cpp -o CMakeFiles/test_detectROI.dir/test_detectROI.cpp.s

CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o.requires:

.PHONY : CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o.requires

CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o.provides: CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o.requires
	$(MAKE) -f CMakeFiles/test_detectROI.dir/build.make CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o.provides.build
.PHONY : CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o.provides

CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o.provides.build: CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o


# Object files for target test_detectROI
test_detectROI_OBJECTS = \
"CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o"

# External object files for target test_detectROI
test_detectROI_EXTERNAL_OBJECTS =

test_detectROI: CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o
test_detectROI: CMakeFiles/test_detectROI.dir/build.make
test_detectROI: /usr/local/lib/libopencv_calib3d.a
test_detectROI: /usr/local/lib/libopencv_core.a
test_detectROI: /usr/local/lib/libopencv_features2d.a
test_detectROI: /usr/local/lib/libopencv_flann.a
test_detectROI: /usr/local/lib/libopencv_highgui.a
test_detectROI: /usr/local/lib/libopencv_imgcodecs.a
test_detectROI: /usr/local/lib/libopencv_imgproc.a
test_detectROI: /usr/local/lib/libopencv_ml.a
test_detectROI: /usr/local/lib/libopencv_objdetect.a
test_detectROI: /usr/local/lib/libopencv_photo.a
test_detectROI: /usr/local/lib/libopencv_shape.a
test_detectROI: /usr/local/lib/libopencv_stitching.a
test_detectROI: /usr/local/lib/libopencv_superres.a
test_detectROI: /usr/local/lib/libopencv_video.a
test_detectROI: /usr/local/lib/libopencv_videoio.a
test_detectROI: /usr/local/lib/libopencv_videostab.a
test_detectROI: /usr/local/lib/libopencv_objdetect.a
test_detectROI: /usr/local/lib/libopencv_calib3d.a
test_detectROI: /usr/local/lib/libopencv_features2d.a
test_detectROI: /usr/local/lib/libopencv_flann.a
test_detectROI: /usr/local/lib/libopencv_highgui.a
test_detectROI: /usr/local/lib/libopencv_ml.a
test_detectROI: /usr/local/lib/libopencv_photo.a
test_detectROI: /usr/local/lib/libopencv_video.a
test_detectROI: /usr/local/lib/libopencv_videoio.a
test_detectROI: /usr/local/lib/libopencv_imgcodecs.a
test_detectROI: /usr/local/share/OpenCV/3rdparty/lib/liblibjpeg.a
test_detectROI: /usr/local/share/OpenCV/3rdparty/lib/liblibwebp.a
test_detectROI: /usr/local/share/OpenCV/3rdparty/lib/liblibpng.a
test_detectROI: /usr/local/share/OpenCV/3rdparty/lib/liblibtiff.a
test_detectROI: /usr/local/share/OpenCV/3rdparty/lib/liblibjasper.a
test_detectROI: /usr/local/share/OpenCV/3rdparty/lib/libIlmImf.a
test_detectROI: /usr/local/lib/libopencv_imgproc.a
test_detectROI: /usr/local/lib/libopencv_core.a
test_detectROI: /usr/local/share/OpenCV/3rdparty/lib/libzlib.a
test_detectROI: /usr/local/share/OpenCV/3rdparty/lib/libippicv.a
test_detectROI: CMakeFiles/test_detectROI.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_detectROI"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_detectROI.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_detectROI.dir/build: test_detectROI

.PHONY : CMakeFiles/test_detectROI.dir/build

CMakeFiles/test_detectROI.dir/requires: CMakeFiles/test_detectROI.dir/test_detectROI.cpp.o.requires

.PHONY : CMakeFiles/test_detectROI.dir/requires

CMakeFiles/test_detectROI.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_detectROI.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_detectROI.dir/clean

CMakeFiles/test_detectROI.dir/depend:
	cd /Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI /Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI /Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI /Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI /Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI/CMakeFiles/test_detectROI.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_detectROI.dir/depend
