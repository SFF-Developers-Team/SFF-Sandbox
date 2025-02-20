@echo off
setlocal enabledelayedexpansion

set ANDROID_PLATFORM=21
set BUILD_TOOLS=%ANDROID_SDK%/build-tools/34.0.0
set ANDROID_ARCH=arm64-v8a


if not exist build-android mkdir build-android
cd build-android

cmake .. ^
  -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake ^
  -DPLATFORM=Android ^
  -DANDROID_ABI=%ANDROID_ARCH% ^
  -DANDROID_NDK=%ANDROID_NDK% ^
  -DANDROID_API_VERSION=%ANDROID_PLATFORM% ^
  -DANDROID_PLATFORM=android-%ANDROID_PLATFORM% ^
  -DANDROID_STL=c++_static ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_C_COMPILER=%ANDROID_NDK%/toolchains/llvm/prebuilt/windows-x86_64/bin/clang.exe ^
  -DCMAKE_CXX_COMPILER=%ANDROID_NDK%/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe ^
  -DCMAKE_MAKE_PROGRAM=%ANDROID_NDK%/prebuilt/windows-x86_64/bin/make.exe ^
  -G "MinGW Makefiles"

cmake --build . --target SFFSandbox -- -j%NUMBER_OF_PROCESSORS%

@rem Generate storekey for APK signing
if not exist SFFSandbox.keystore keytool -genkeypair -validity 10000 -dname "CN=sff,O=Android,C=ES" -keystore SFFSandbox.keystore -storepass raylib -keypass raylib -alias SFFSandboxKey -keyalg RSA

@rem Config project package and resource using AndroidManifest.xml and res/values/strings.xml
%BUILD_TOOLS%/aapt package -f -m -S ./apk/res -J ./apk/src -M ./apk/AndroidManifest.xml -I %ANDROID_SDK%/platforms/android-%ANDROID_PLATFORM%/android.jar

@rem Compile project .java code into .class (Java bytecode) 
javac -verbose --source 11 --target 11 -d ./obj --system %JAVA_HOME% --class-path %ANDROID_SDK%/platforms/android-%ANDROID_PLATFORM%/android.jar;./obj --source-path ./apk/src ./apk/src/com/sff/sandbox/R.java ./apk/src/com/sff/sandbox/NativeLoader.java

@rem Compile .class files into Dalvik executable bytecode (.dex)
%BUILD_TOOLS%/d8 ./obj/com/sff/sandbox/*.class --release --output ./bin --lib %ANDROID_SDK%/platforms/android-%ANDROID_PLATFORM%/android.jar

@rem Create Android APK package
%BUILD_TOOLS%/aapt package -f -M ./apk/AndroidManifest.xml -S ./apk/res -A ./apk/assets -I %ANDROID_SDK%/platforms/android-%ANDROID_PLATFORM%/android.jar -F ./bin/SFFSandbox.unaligned.apk ./bin
%BUILD_TOOLS%/aapt add ./bin/SFFSandbox.unaligned.apk ./apk/lib/%ANDROID_ARCH%/libSFFSandbox.so

@rem Create zip-aligned APK package
%BUILD_TOOLS%/zipalign -p -f 4 ./bin/SFFSandbox.unaligned.apk ./bin/SFFSandbox.aligned.apk

@rem Create signed APK package using generated Key
%BUILD_TOOLS%/apksigner sign --ks ./SFFSandbox.keystore --ks-pass pass:raylib --key-pass pass:raylib --out SFFSandbox.apk --ks-key-alias SFFSandboxKey ./bin/SFFSandbox.aligned.apk

cd ..