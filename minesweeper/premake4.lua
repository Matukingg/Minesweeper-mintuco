
solution "MiBuscaminas"
   configurations { "Debug", "Release" }
   location "build"
   language "C++"

project "buscaminas"
   kind "ConsoleApp"
   language "C++"
   files { "src/**.h", "src/**.cpp" }
   includedirs {
      "src",
      "src/core",
      "src/graphics",
      "src/input",
      "src/tiles",
      "src/utils",
      "src/leaderboard"
   }
   
   links{
      "allegro",
      "allegro_image",
      "allegro_font",
      "allegro_ttf",
      "allegro_primitives",
      "allegro_audio",
   }
   
   configuration "Debug"
      defines { "DEBUG" }
      flags { "Symbols" }
      targetdir "build/bin/debug"
      objdir    "build/obj/debug"

   configuration "Release"
      defines { "NDEBUG" }
      flags { "Optimize" }
      targetdir "build/bin/release"
      objdir    "build/obj/release"
