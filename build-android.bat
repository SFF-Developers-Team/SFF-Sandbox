@echo off
setlocal enabledelayedexpansion

set ANDROID_PLATFORM=21


if not exist build-android mkdir build-android
cd build-android

cmake .. ^
  -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake ^
  -DPLATFORM=Android ^
  -DANDROID_ABI=arm64-v8a ^
  -DANDROID_NDK=%ANDROID_NDK% ^
  -DANDROID_PLATFORM=android-%ANDROID_PLATFORM% ^
  -DLIBCXX_ENABLE_INCOMPLETE_FEATURES ^
  -DANDROID_STL=c++_static ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_C_COMPILER=%ANDROID_NDK%/toolchains/llvm/prebuilt/windows-x86_64/bin/clang.exe ^
  -DCMAKE_CXX_COMPILER=%ANDROID_NDK%/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe ^
  -G Ninja

cmake --build . --target SFFSandbox -- -j%NUMBER_OF_PROCESSORS%

cd ..
