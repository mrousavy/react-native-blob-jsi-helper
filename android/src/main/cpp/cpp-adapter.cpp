#include <jni.h>
#include <jsi/jsi.h>
#include <string>
#include "../cpp/JSI Utils/TypedArray.h"
#include <android/log.h>

using namespace facebook;

std::string getPropertyAsStringOrEmptyFromObject(jsi::Object& object, const std::string& propertyName, jsi::Runtime& runtime) {
    jsi::Value value = object.getProperty(runtime, propertyName.c_str());
    return value.isString() ? value.asString(runtime).utf8(runtime) : "";
}

typedef u_int8_t byte;

void install(jsi::Runtime& jsiRuntime, std::function<byte*(const std::string& blobId, int offset, int size)> getBytesFromBlob) {
    // getArrayBufferForBlobId()
    auto getArrayBufferForBlobId = jsi::Function::createFromHostFunction(jsiRuntime,
                                                                       jsi::PropNameID::forAscii(jsiRuntime, "getArrayBufferForBlobId"),
                                                                       1,
                                                                       [=](jsi::Runtime& runtime,
                                                                           const jsi::Value& thisValue,
                                                                           const jsi::Value* arguments,
                                                                           size_t count) -> jsi::Value {
        if (count != 1) {
            throw jsi::JSError(runtime, "getArrayBufferForBlobId(..) expects one argument (object)!");
        }

        jsi::Object data = arguments[0].asObject(runtime);
        auto blobId = data.getProperty(runtime, "blobId").asString(runtime);
        auto offset = data.getProperty(runtime, "offset").asNumber();
        auto size = data.getProperty(runtime, "size").asNumber();

        auto bytes = getBytesFromBlob(blobId.utf8(runtime), offset, size);

        size_t totalSize = size - offset;
        auto typedArray = TypedArray<TypedArrayKind::Uint8Array>(runtime, totalSize);
        auto arrayBuffer = typedArray.getBuffer(runtime);
                                                                           __android_log_print(ANDROID_LOG_INFO, "RNBLOBJSIHELPER", "Copying memory...");

        memcpy(arrayBuffer.data(runtime), reinterpret_cast<byte*>(bytes), 5);
                                                                           __android_log_print(ANDROID_LOG_INFO, "RNBLOBJSIHELPER", "Returned!");

        return typedArray;
    });
    jsiRuntime.global().setProperty(jsiRuntime, "getArrayBufferForBlobId", std::move(getArrayBufferForBlobId));
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
        env->ReleaseStringUTFChars(jstring, blobId.c_str());

        jboolean isCopy = true;
        jbyte* bytes = env->GetByteArrayElements(boxedBytes, &isCopy);
        // TODO: Clean this up later? idk...
        return reinterpret_cast<byte*>(bytes);
    };

    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    if (runtime) {
        install(*runtime, getBytesFromBlob);
    }
    // if runtime was nullptr, the helper will not be installed. This should only happen while Remote Debugging (Chrome), but will be weird either way.
}
