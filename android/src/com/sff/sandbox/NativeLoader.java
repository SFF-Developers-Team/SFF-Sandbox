package com.sff.sandbox;

public class NativeLoader extends android.app.NativeActivity {
    static {
        System.loadLibrary("SFFSandbox");
    }
}
