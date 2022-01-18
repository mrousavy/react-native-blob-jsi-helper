#include <jni.h>
#include <jsi/jsi.h>

using namespace facebook;

std::string getPropertyAsStringOrEmptyFromObject(jsi::Object& object, const std::string& propertyName, jsi::Runtime& runtime) {
    jsi::Value value = object.getProperty(runtime, propertyName.c_str());
    return value.isString() ? value.asString(runtime).utf8(runtime) : "";
}

void install(jsi::Runtime& jsiRuntime, JNIEnv* env) {
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
    auto getBuffer = [=]() {
        if (!env) throw std::runtime_error("JNI Environment is gone!");

        jclass clazz = env->GetObjectClass(env, instance);
        jmethodID getBufferJava = env->GetMethodID(env, clazz, "getBlobBuffer", "()V");
        env->CallVoidMethod(env, instance, getBufferJava);
    };


    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    if (runtime) {
        install(*runtime, env);
    }
    // if runtime was nullptr, the helper will not be installed. This should only happen while Remote Debugging (Chrome), but will be weird either way.
}
