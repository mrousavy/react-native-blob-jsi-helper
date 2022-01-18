package com.reactnativeblobjsihelper;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;

@ReactModule(name = BlobJsiHelperModule.NAME)
public class BlobJsiHelperModule extends ReactContextBaseJavaModule {
    public static final String NAME = "BlobJsiHelper";

    public BlobJsiHelperModule(ReactApplicationContext reactContext) {
        super(reactContext);
    }

    @Override
    @NonNull
    public String getName() {
        return NAME;
    }

    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean install() {
        try {
            System.loadLibrary("blobjsihelper");
            nativeInstall();
            return true;
        } catch (Exception ignored) {
            return false;
        }
    }

    public static native void nativeInstall();
}
