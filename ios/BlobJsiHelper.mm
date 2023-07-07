#import "BlobJsiHelper.h"
#import <React/RCTBlobManager.h>
#import <React/RCTBridge+Private.h>
#import <ReactCommon/RCTTurboModule.h>
#import <jsi/jsi.h>
#import "../cpp/JSI Utils/TypedArray.h"

@implementation BlobJsiHelper

RCT_EXPORT_MODULE()

RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(install)
{
  NSLog(@"Installing ArrayBuffer <-> Blob Bindings...");
  RCTBridge* bridge = [RCTBridge currentBridge];
  RCTCxxBridge* cxxBridge = (RCTCxxBridge*)bridge;
  if (cxxBridge == nil) {
    return @false;
  }
  
  using namespace facebook;
  
  auto jsiRuntime = (jsi::Runtime*) cxxBridge.runtime;
  if (jsiRuntime == nil) {
    return @false;
  }
  auto& runtime = *jsiRuntime;
  
  auto getArrayBufferForBlob = jsi::Function::createFromHostFunction(runtime,
                                                                     jsi::PropNameID::forUtf8(runtime, "getArrayBufferForBlob"),
                                                                     1,
                                                                     [](jsi::Runtime& runtime,
                                                                        const jsi::Value& thisArg,
                                                                        const jsi::Value* args,
                                                                        size_t count) -> jsi::Value {
    auto data = args[0].asObject(runtime);
    auto blobId = data.getProperty(runtime, "blobId").asString(runtime).utf8(runtime);
    auto size = data.getProperty(runtime, "size").asNumber();
    auto offset = data.getProperty(runtime, "offset").asNumber();
    
    RCTBlobManager* blobManager = [[RCTBridge currentBridge] moduleForClass:RCTBlobManager.class];
    auto blob = [blobManager resolve:[NSString stringWithUTF8String:blobId.c_str()] offset:(long)offset size:(long)size];
    
    auto typedArray = TypedArray<TypedArrayKind::Uint8Array>(runtime, blob.length);
    auto arrayBuffer = typedArray.getBuffer(runtime);
    memcpy(arrayBuffer.data(runtime), blob.bytes, blob.length);
    
    return typedArray;
  });
  runtime.global().setProperty(runtime, "getArrayBufferForBlob", getArrayBufferForBlob);
  
  auto getBlobForArrayBuffer = jsi::Function::createFromHostFunction(runtime,
                                                                     jsi::PropNameID::forUtf8(runtime, "getBlobForArrayBuffer"),
                                                                     1,
                                                                     [](jsi::Runtime& runtime,
                                                                        const jsi::Value& thisArg,
                                                                        const jsi::Value* args,
                                                                        size_t count) -> jsi::Value {
    auto arrayBuffer = args[0].asObject(runtime).getArrayBuffer(runtime);
    auto size = arrayBuffer.length(runtime);
    NSData* data = [NSData dataWithBytes:arrayBuffer.data(runtime) length:size];
    
    RCTBlobManager* blobManager = [[RCTBridge currentBridge] moduleForClass:RCTBlobManager.class];
    NSString* blobId = [blobManager store:data];
    
    jsi::Object result(runtime);
    auto blobIdString = jsi::String::createFromUtf8(runtime, [blobId cStringUsingEncoding:NSUTF8StringEncoding]);
    result.setProperty(runtime, "blobId", blobIdString);
    result.setProperty(runtime, "offset", jsi::Value(0));
    result.setProperty(runtime, "size", jsi::Value(static_cast<double>(size)));
    return result;
  });
  runtime.global().setProperty(runtime, "getBlobForArrayBuffer", getBlobForArrayBuffer);
  
  jsi::Object cacheCleaner = jsi::Object::createFromHostObject(runtime,
                                                               std::make_shared<InvalidateCacheOnDestroy>(runtime));
  runtime.global().setProperty(runtime, "__blobJSIHelperCacheCleaner", cacheCleaner);
  
  NSLog(@"Installed ArrayBuffer <-> Blob Bindings!");
  return @true;
}

@end
