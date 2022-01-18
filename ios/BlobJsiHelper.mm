#import "BlobJsiHelper.h"
#import "react-native-blob-jsi-helper.h"
#import <React/RCTBlobManager.h>
#import <React/RCTBridge+Private.h>
#import <ReactCommon/RCTTurboModule.h>
#import <jsi/jsi.h>
#import "../cpp/JSI Utils/TypedArray.h"

@implementation BlobJsiHelper

RCT_EXPORT_MODULE()

RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(install)
{
  NSLog(@"Installing global.getArrayBufferForBlobId...");
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
  
  auto func = jsi::Function::createFromHostFunction(runtime,
                                                    jsi::PropNameID::forUtf8(runtime, "getArrayBufferForBlobId"),
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
  runtime.global().setProperty(runtime, "getArrayBufferForBlobId", func);
  
  NSLog(@"Installed global.getArrayBufferForBlobId!");
  return @true;
}

@end
