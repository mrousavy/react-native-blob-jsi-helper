#include <jni.h>
#include <jsi/jsi.h>
#include <string>

using namespace facebook;

std::string getPropertyAsStringOrEmptyFromObject(jsi::Object& object, const std::string& propertyName, jsi::Runtime& runtime) {
    jsi::Value value = object.getProperty(runtime, propertyName.c_str());
    return value.isString() ? value.asString(runtime).utf8(runtime) : "";
}

typedef u_int8_t byte;

void install(jsi::Runtime& jsiRuntime, std::function<byte*(std::string blobId, int offset, int size)> getBytesFromBlob) {
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
                                                                         jsi::Object config = arguments[0].asObject(runtime);

                                                                         return jsi::Value::undefined();
                                                                       });
    jsiRuntime.global().setProperty(jsiRuntime, "getArrayBufferForBlobId", std::move(getArrayBufferForBlobId));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativeblobjsihelper_BlobJsiHelperModule_nativeInstall(JNIEnv *env, jobject _, jlong jsiPtr, jobject instance) {
    auto getBytesFromBlob = [=](std::string blobId, int offset, int size) -> byte* {
        if (!env) throw std::runtime_error("JNI Environment is gone!");

        jclass clazz = env->GetObjectClass(instance);
        jmethodID getBufferJava = env->GetMethodID(clazz, "getBlobBuffer", "()[B");
        jobject boxedBytes = env->CallObjectMethod(instance,
                                                   getBufferJava,
                                                   // arguments
                                                   env->NewStringUTF(blobId.c_str()),
                                                   offset,
                                                   size);
        jbyte* bytes = (jbyte*)boxedBytes;
        return (byte*)bytes;
    };

    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    if (runtime) {
        install(*runtime, getBytesFromBlob);
    }
    // if runtime was nullptr, the helper will not be installed. This should only happen while Remote Debugging (Chrome), but will be weird either way.
}
