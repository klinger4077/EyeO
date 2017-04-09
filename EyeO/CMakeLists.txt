cmake_minimum_required(VERSION 2.8.12)
# Every project needs a name.  We call this the "examples" project.
project(examples)


# Tell cmake we will need dlib.  This command will pull in dlib and compile it
# into your project.  Note that you don't need to compile or install dlib.  All
# it needs is the dlib source code folder and it will take care of everything.
include(../dlib/cmake)


# Locate libraries and headers
find_package(WiringPi REQUIRED)

if (DLIB_NO_GUI_SUPPORT)
	message("No GUI support, so we can't build EyeO.")
else()
   find_package(OpenCV QUIET)
   if (OpenCV_FOUND)
      include_directories(${OpenCV_INCLUDE_DIRS})

		add_executable(EyeO EyeO.cpp)
		target_link_libraries(EyeO dlib::dlib ${OpenCV_LIBS} )

		target_link_libraries(EyeO ${WIRINGPI_LIBRARIES}) 
   else()
      message("OpenCV not found, so we won't build the webcam_face_pose_ex example.")
   endif()
endif()

