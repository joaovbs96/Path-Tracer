cmake_minimum_required(VERSION 3.0.0)
project(Path-Tracer VERSION 2.0.0)

add_executable(Path-Tracer
  # libs
  src/lib/HDRloader.cpp
  src/lib/tiny_obj_loader.cc

  # main file
  src/main.cpp
)
