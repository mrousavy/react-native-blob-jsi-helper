#include <jni.h>
#include <jsi/jsi.h>

using namespace facebook;

std::string getPropertyAsStringOrEmptyFromObject(jsi::Object& object, const std::string& propertyName, jsi::Runtime& runtime) {
    jsi::Value value = object.getProperty(runtime, propertyName.c_str());
    return value.isString() ? value.asString(runtime).utf8(runtime) : "";
}

void install(jsi::Runtime& jsiRuntime) {
    // getArrayBufferForBlobId()
    auto getArrayBufferForBlobId = jsi::Function::createFromHostFunction(jsiRuntime,
                                                                       jsi::PropNameID::forAscii(jsiRuntime, "getArrayBufferForBlobId"),
                                                                       1,
                                                                       [](jsi::Runtime& runtime,
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
Java_com_reactnativeblobljsihelper_BlobJsiHelperModule_nativeInstall(JNIEnv *env, jobject clazz, jlong jsiPtr, jstring path) {
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    if (runtime) {
        install(*runtime);
    }
    // if runtime was nullptr, the helper will not be installed. This should only happen while Remote Debugging (Chrome), but will be weird either way.
}
