#import "BlobJsiHelper.h"
#import "react-native-blob-jsi-helper.h"
#import <React/RCTBlobManager.h>
#import <React/RCTBridge+Private.h>
#import <ReactCommon/RCTTurboModule.h>
#import <jsi/jsi.h>

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
    return jsi::Value(5);
  });
  runtime.global().setProperty(runtime, "getArrayBufferForBlobId", func);
  
  NSLog(@"Installed global.getArrayBufferForBlobId!");
  return @true;
}

@end
