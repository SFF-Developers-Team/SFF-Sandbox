@echo off
setlocal enabledelayedexpansion

set ANDROID_ARCH=arm64-v8a
set MIN_API_VERSION=21
set API_VERSION=29
set DEV_NAME=sff
set PACKAGE_NAME=sandbox
set PROJECT_NAME=SFFSandbox
set LIB_NAME=main
set STOREPASS=raylib

set BUILD_TOOLS_VERSION=34.0.0
set BUILD_DIR=build-android
set OUT_DIR=out
set OUT_PATH=%BUILD_DIR%\%OUT_DIR%
set BUILD_TOOLS=%ANDROID_SDK%/build-tools/%BUILD_TOOLS_VERSION%
set PLATFORM_TOOLS=%ANDROID_SDK%/platform-tools
set SDK_MANAGER=%ANDROID_SDK%/cmdline-tools/bin/sdkmanager.bat
set PLATFORM_DIR=%ANDROID_SDK%/platforms/android-%API_VERSION%

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

if %JAVA_HOME% == "" (
  echo JAVA_HOME isn't set!
  echo Please download OpenJDK and set JAVA_HOME to root directory of OpenJDK.
  exit /b 1
)

if %ANDROID_SDK% == "" (
  echo ANDROID_SDK not found! Downloading...
  set ANDROID_SDK=%BUILD_DIR%/android-sdk

  if not exist %ANDROID_SDK% mkdir %ANDROID_SDK%
  
  powershell -command "Start-BitsTransfer -Source https://dl.google.com/android/repository/commandlinetools-win-8092744_latest.zip -Destination %ANDROID_SDK%/android-sdk.zip"
  if %errorlevel% neq 0 (
    echo Failed to download android-sdk!
    exit /b %errorlevel%
  )  

  powershell -command "Expand-Archive %ANDROID_SDK%/android-sdk.zip %ANDROID_SDK%"

  if %errorlevel% neq 0 (
    echo Failed to unzip android-sdk.zip!
    exit /b %errorlevel%
  )  
)

if %ANDROID_NDK% == "" (
  echo ANDROID_NDK not found! Downloading...
  set ANDROID_NDK=%BUILD_DIR%/android-ndk

  if not exist %ANDROID_NDK% mkdir %ANDROID_NDK%
  
  powershell -command "Start-BitsTransfer -Source https://dl.google.com/android/repository/android-ndk-r28-win.zip -Destination %ANDROID_NDK%/android-ndk.zip"
  if %errorlevel% neq 0 (
    echo Failed to download android-ndk!
    exit /b %errorlevel%
  )  

  powershell -command "Expand-Archive %ANDROID_NDK%/android-ndk.zip %ANDROID_NDK%"

  if %errorlevel% neq 0 (
    echo Failed to unzip android-ndk.zip!
    exit /b %errorlevel%
  )  
)

if not exist %BUILD_TOOLS% (
  echo %BUILD_TOOLS% not found! Downloading...
  call %SDK_MANAGER% --install "build-tools;%BUILD_TOOLS_VERSION%" --sdk_root=%ANDROID_SDK%
)

if not exist %PLATFORM_DIR% (
  echo %PLATFORM_DIR% not found! Downloading...
  call %SDK_MANAGER% --install "platforms;android-%API_VERSION%" --sdk_root=%ANDROID_SDK%
)

if not exist %PLATFORM_TOOLS% (
  echo %PLATFORM_TOOLS% not found! Downloading...
  call %SDK_MANAGER% --install "platform-tools" --sdk_root=%ANDROID_SDK%
)

del "%OUT_PATH%\*" /s /q
del "%BUILD_DIR%\%PROJECT_NAME%.apk" /f /s /q

if %ANDROID_ARCH% == armeabi-v7a set ABI_FLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16"
if %ANDROID_ARCH% == arm64-v8a set ABI_FLAGS="-target aarch64 -mfix-cortex-a53-835769"
  
cmake . ^
  -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake ^
  -DPLATFORM=Android ^
  -DANDROID_ABI=%ANDROID_ARCH% ^
  -DANDROID_NDK=%ANDROID_NDK% ^
  -DANDROID_PLATFORM=android-%API_VERSION% ^
  -DANDROID_API_VERSION=%API_VERSION% ^
  -DANDROID_API_VERSION_MIN=%MIN_API_VERSION% ^
  -DCMAKE_ANDROID_STL_TYPE=c++_static ^
  -DOUT_DIR=%OUT_DIR% ^
  -DLIB_NAME=%LIB_NAME% ^
  -DDEV_NAME=%DEV_NAME% ^
  -DPACKAGE_NAME=%PACKAGE_NAME% ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_C_COMPILER=%ANDROID_NDK%/toolchains/llvm/prebuilt/windows-x86_64/bin/clang.exe ^
  -DCMAKE_CXX_COMPILER=%ANDROID_NDK%/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe ^
  -DCMAKE_MAKE_PROGRAM=%ANDROID_NDK%/prebuilt/windows-x86_64/bin/make.exe ^
  -G "MinGW Makefiles" ^
  -B%BUILD_DIR%

cmake --build %BUILD_DIR% --target SFFSandbox -- -j%NUMBER_OF_PROCESSORS%

if %errorlevel% neq 0 exit /b %errorlevel%

if not exist %BUILD_DIR%/%PROJECT_NAME%.keystore %JAVA_HOME%/bin/keytool ^
  -genkeypair -validity 10000 -dname "CN=%DEV_NAME%,O=Android,C=ES" ^
  -keystore %BUILD_DIR%/%PROJECT_NAME%.keystore ^
  -storepass %STOREPASS% -keypass %STOREPASS% -alias %PROJECT_NAME%Key -keyalg RSA

@REM Config project package and resource using AndroidManifest.xml and res/values/strings.xml
call %BUILD_TOOLS%/aapt package -f -m ^
  -S %OUT_PATH%/res ^
  -J %OUT_PATH%/src ^
  -M %OUT_PATH%/AndroidManifest.xml ^
  -I %ANDROID_SDK%/platforms/android-%API_VERSION%/android.jar


@REM Compile NativeLoader.java
call %JAVA_HOME%/bin/javac -verbose --source 11 --target 11 -d %OUT_PATH%/obj ^
  --system %JAVA_HOME% ^
	--class-path %ANDROID_SDK%/platforms/android-%API_VERSION%/android.jar;%OUT_PATH%/obj ^
	--source-path %OUT_PATH%/src %OUT_PATH%/src/com/%DEV_NAME%/%PACKAGE_NAME%/R.java ^
	%OUT_PATH%/src/com/%DEV_NAME%/%PACKAGE_NAME%/NativeLoader.java

call %BUILD_TOOLS%/d8 ^
  %OUT_PATH%/obj/com/%DEV_NAME%/%PACKAGE_NAME%/*.class ^
  --release --output %OUT_PATH%/bin ^
  --lib %ANDROID_SDK%/platforms/android-%API_VERSION%/android.jar

@REM Add resources and assets to APK
call %BUILD_TOOLS%/aapt package -f ^
  -M %OUT_PATH%/AndroidManifest.xml ^
  -S %OUT_PATH%/res -A %BUILD_DIR%/assets ^
  -I %ANDROID_SDK%/platforms/android-%API_VERSION%/android.jar ^
  -F %OUT_PATH%/bin/%PROJECT_NAME%.unaligned.apk %OUT_PATH%/bin

@REM Add library to APK
set CURRENT_DIR=%~dp0
cd %OUT_PATH%
call %BUILD_TOOLS%/aapt add bin/%PROJECT_NAME%.unaligned.apk lib/%ANDROID_ARCH%/lib%LIB_NAME%.so
cd "%CURRENT_DIR%"

@REM Sign and zipalign APK
%JAVA_HOME%/bin/jarsigner -keystore %BUILD_DIR%/%PROJECT_NAME%.keystore -storepass %STOREPASS% -keypass %STOREPASS% ^
	-signedjar %OUT_PATH%/bin/%PROJECT_NAME%.signed.apk %OUT_PATH%/bin/%PROJECT_NAME%.unaligned.apk %PROJECT_NAME%Key

call %BUILD_TOOLS%/zipalign -f 4 %OUT_PATH%/bin/%PROJECT_NAME%.signed.apk %BUILD_DIR%/%PROJECT_NAME%.apk

@REM Install to device or emulator if -deploy was specified
@REM if %%1 == "-deploy" (
  %PLATFORM_TOOLS%/adb uninstall com.sff.sandbox 
  %PLATFORM_TOOLS%/adb install %BUILD_DIR%/%PROJECT_NAME%.apk
@REM )