@echo off
setlocal enabledelayedexpansion

set ANDROID_PLATFORM=29
set BUILD_TOOLS=%ANDROID_SDK%/build-tools/34.0.0
set ANDROID_ARCH=arm64-v8a

set BUILD_DIR=build-android
set PROJECT_NAME=sffsandbox
set STOREPASS=raylib


rmdir %BUILD_DIR%\bin /s /q
rmdir %BUILD_DIR%\assets /s /q
rmdir %BUILD_DIR%\lib /s /q 
rmdir %BUILD_DIR%\obj /s /q
rmdir %BUILD_DIR%\res /s /q
rmdir %BUILD_DIR%\src /s /q
del %BUILD_DIR%\AndroidManifest.xml /f /s /q
del %BUILD_DIR%\%PROJECT_NAME%.apk /f /s /q
del %BUILD_DIR%\%PROJECT_NAME%.keystore /f /s /q

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

cmake . ^
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
  -G "MinGW Makefiles" ^
  -B%BUILD_DIR%

cmake --build %BUILD_DIR% --target SFFSandbox -- -j%NUMBER_OF_PROCESSORS%

if %errorlevel% neq 0 exit /b %errorlevel%

cd %BUILD_DIR%

echo f | xcopy /f /y libs\enet\libenet_static.a lib\%ANDROID_ARCH%\libenet_static.a
echo f | xcopy /f /y libs\GitHash\libgithash.a lib\%ANDROID_ARCH%\libgithash.a
echo f | xcopy /f /y libs\miniz\libminiz.a lib\%ANDROID_ARCH%\libminiz.a
echo f | xcopy /f /y libs\raylib\raylib\libraylib.a lib\%ANDROID_ARCH%\libraylib.a

@REM Generate storekey for APK signing
if not exist %PROJECT_NAME%.keystore keytool -genkeypair -validity 10000 -dname "CN=sff,O=Android,C=ES" -keystore %PROJECT_NAME%.keystore -storepass %STOREPASS% -keypass %STOREPASS% -alias %PROJECT_NAME%Key -keyalg RSA

@rem Config project package and resource using AndroidManifest.xml and res/values/strings.xml
call %BUILD_TOOLS%/aapt package -f -m -S res -J src -M AndroidManifest.xml -I %ANDROID_SDK%/platforms/android-%ANDROID_PLATFORM%/android.jar

@rem Compile project .java code into .class (Java bytecode) 
javac -verbose --source 11 --target 11 -d obj --system %JAVA_HOME% --class-path "%ANDROID_SDK%/platforms/android-%ANDROID_PLATFORM%/android.jar;obj" --source-path src src/com/sff/sandbox/R.java src/com/sff/sandbox/NativeLoader.java

@rem Compile .class files into Dalvik executable bytecode (.dex)
call %BUILD_TOOLS%/d8 obj/com/sff/sandbox/*.class --release --output bin --lib %ANDROID_SDK%/platforms/android-%ANDROID_PLATFORM%/android.jar

@rem Create Android APK package
call %BUILD_TOOLS%/aapt package -f -M AndroidManifest.xml -S res -A assets -I %ANDROID_SDK%/platforms/android-%ANDROID_PLATFORM%/android.jar -F bin/%PROJECT_NAME%.unaligned.apk bin
call %BUILD_TOOLS%/aapt add bin/%PROJECT_NAME%.unaligned.apk lib/%ANDROID_ARCH%/lib%PROJECT_NAME%.so lib/%ANDROID_ARCH%/libraylib.a lib/%ANDROID_ARCH%/libenet_static.a lib/%ANDROID_ARCH%/libgithash.a lib/%ANDROID_ARCH%/libminiz.a

@rem Create zip-aligned APK package
call %BUILD_TOOLS%/zipalign -p -f 4 bin/%PROJECT_NAME%.unaligned.apk bin/%PROJECT_NAME%.aligned.apk

@rem Create signed APK package using generated Key
call %BUILD_TOOLS%/apksigner sign --ks %PROJECT_NAME%.keystore --ks-pass pass:%STOREPASS% --key-pass pass:%STOREPASS% --out %PROJECT_NAME%.apk --ks-key-alias %PROJECT_NAME%Key bin/%PROJECT_NAME%.aligned.apk