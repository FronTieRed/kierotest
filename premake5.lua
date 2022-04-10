-- premake5.lua
workspace "KieroTest"
   configurations { "Debug", "Release" }

project "KieroTest"
   kind "SharedLib"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   files { "**.h", "**.cpp", "**.c" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"