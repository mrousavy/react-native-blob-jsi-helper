#include <jni.h>
#include <jsi/jsi.h>
#include <string>
#include "../cpp/JSI Utils/TypedArray.h"
#include <android/log.h>

using namespace facebook;

typedef u_int8_t byte;

void install(jsi::Runtime& jsiRuntime,
             std::function<byte*(const std::string& blobId, int offset, int size)> getBytesFromBlob,
             std::function<std::string(byte* bytes, size_t size)> createBlob) {
    // getArrayBufferForBlob()
    auto getArrayBufferForBlob = jsi::Function::createFromHostFunction(jsiRuntime,
                                                                       jsi::PropNameID::forAscii(jsiRuntime, "getArrayBufferForBlob"),
                                                                       1,
                                                                       [=](jsi::Runtime& runtime,
                                                                         const jsi::Value& thisValue,
                                                                         const jsi::Value* arguments,
                                                                         size_t count) -> jsi::Value {
        if (count != 1) {
            throw jsi::JSError(runtime, "getArrayBufferForBlob(..) expects one argument (object)!");
        }

        jsi::Object data = arguments[0].asObject(runtime);
        auto blobId = data.getProperty(runtime, "blobId").asString(runtime);
        auto offset = data.getProperty(runtime, "offset").asNumber();
        auto size = data.getProperty(runtime, "size").asNumber();

        __android_log_print(ANDROID_LOG_INFO, "RNBLOBJSIHELPER", "Getting Blob bytes...");
        auto bytes = getBytesFromBlob(blobId.utf8(runtime), offset, size);
        __android_log_print(ANDROID_LOG_INFO, "RNBLOBJSIHELPER", "Got Blob bytes!");

        size_t totalSize = size - offset;
        auto typedArray = TypedArray<TypedArrayKind::Uint8Array>(runtime, totalSize);
        auto arrayBuffer = typedArray.getBuffer(runtime);
        __android_log_print(ANDROID_LOG_INFO, "RNBLOBJSIHELPER", "Copying memory...");

        memcpy(arrayBuffer.data(runtime), reinterpret_cast<byte*>(bytes), totalSize);
        __android_log_print(ANDROID_LOG_INFO, "RNBLOBJSIHELPER", "Returned!");

        return typedArray;
    });
    jsiRuntime.global().setProperty(jsiRuntime, "getArrayBufferForBlob", std::move(getArrayBufferForBlob));

    // getBlobForArrayBuffer()
    auto getBlobForArrayBuffer = jsi::Function::createFromHostFunction(jsiRuntime,
                                                                       jsi::PropNameID::forAscii(jsiRuntime, "getBlobForArrayBuffer"),
                                                                       1,
                                                                       [=](jsi::Runtime& runtime,
                                                                           const jsi::Value& thisValue,
                                                                           const jsi::Value* arguments,
                                                                           size_t count) -> jsi::Value {
        if (count != 1) {
            throw jsi::JSError(runtime, "getBlobForArrayBuffer(..) expects one argument (object)!");
        }

        auto arrayBuffer = arguments[0].asObject(runtime).getArrayBuffer(runtime);
        auto size = arrayBuffer.size(runtime);

        __android_log_print(ANDROID_LOG_INFO, "RNBLOBJSIHELPER", "Creating Blob...");
        std::string blobId = createBlob(arrayBuffer.data(runtime), size);
        __android_log_print(ANDROID_LOG_INFO, "RNBLOBJSIHELPER", "Created Blob! Blob ID: %s", blobId.c_str());

        jsi::Object result(runtime);
        auto blobIdString = jsi::String::createFromUtf8(runtime, blobId);
        result.setProperty(runtime, "blobId", blobIdString);
        result.setProperty(runtime, "offset", jsi::Value(0));
        result.setProperty(runtime, "size", jsi::Value(static_cast<double>(size)));
        return result;
    });
    jsiRuntime.global().setProperty(jsiRuntime, "getBlobForArrayBuffer", std::move(getBlobForArrayBuffer));


    jsi::Object cacheCleaner = jsi::Object::createFromHostObject(jsiRuntime,
                                                                std::make_shared<InvalidateCacheOnDestroy>(jsiRuntime));
    jsiRuntime.global().setProperty(jsiRuntime, "__blobJSIHelperCacheCleaner", cacheCleaner);
}

std::string jstring2string(JNIEnv *env, jstring jStr) {
    if (!jStr) return "";

    jclass stringClass = env->GetObjectClass(jStr);
    jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const auto stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

    auto length = (size_t) env->GetArrayLength(stringJbytes);
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, nullptr);

    std::string ret = std::string((char *)pBytes, length);
    __android_log_print(ANDROID_LOG_INFO, "RNBLOBJSIHELPER", "Got std::string");
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    __android_log_print(ANDROID_LOG_INFO, "RNBLOBJSIHELPER", "Yay");
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativeblobjsihelper_BlobJsiHelperModule_nativeInstall(JNIEnv *env, jclass _, jlong jsiPtr, jobject instance) {
    auto instanceGlobal = env->NewGlobalRef(instance);
    auto getBytesFromBlob = [=](const std::string& blobId, int offset, int size) -> byte* {
        if (!env) throw std::runtime_error("JNI Environment is gone!");

        // get java class
        jclass clazz = env->GetObjectClass(instanceGlobal);
        // get method in java class
        jmethodID getBufferJava = env->GetMethodID(clazz, "getBlobBuffer", "(Ljava/lang/String;II)[B");
        // call method
        auto jstring = env->NewStringUTF(blobId.c_str());
        auto boxedBytes = (jbyteArray) env->CallObjectMethod(instanceGlobal,
                                                             getBufferJava,
                                                             // arguments
                                                             jstring,
                                                             offset,
                                                             size);
        env->DeleteLocalRef(jstring);

        jboolean isCopy = true;
        jbyte* bytes = env->GetByteArrayElements(boxedBytes, &isCopy);
        env->DeleteLocalRef(boxedBytes);
        // TODO: Clean this up later? idk...
        return reinterpret_cast<byte*>(bytes);
    };

    auto createBlob = [=](byte* bytes, size_t size) -> std::string {
        if (!env) throw std::runtime_error("JNI Environment is gone!");

        // get java class
        jclass clazz = env->GetObjectClass(instanceGlobal);
        // get method in java class
        jmethodID getBufferJava = env->GetMethodID(clazz, "createBlob", "([B)Ljava/lang/String;");
        // call method
        auto byteArray = env->NewByteArray(size);
        env->SetByteArrayRegion(byteArray, 0, size, reinterpret_cast<jbyte*>(bytes));
        auto blobId = (jstring) env->CallObjectMethod(instanceGlobal,
                                                      getBufferJava,
                                                      // arguments
                                                      byteArray);
        env->DeleteLocalRef(byteArray);

        return jstring2string(env, blobId);
    };

    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    if (runtime) {
        install(*runtime, getBytesFromBlob, createBlob);
    }
    // if runtime was nullptr, the helper will not be installed. This should only happen while Remote Debugging (Chrome), but will be weird either way.
}
