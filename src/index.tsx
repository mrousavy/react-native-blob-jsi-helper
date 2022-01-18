import { NativeModules, Platform } from 'react-native';

const LINKING_ERROR =
  `The package 'react-native-blob-jsi-helper' doesn't seem to be linked. Make sure: \n\n` +
  Platform.select({ ios: "- You have run 'pod install'\n", default: '' }) +
  '- You rebuilt the app after installing the package\n' +
  '- You are not using Expo managed workflow\n';

const BlobJsiHelper = NativeModules.BlobJsiHelper;

if (BlobJsiHelper == null) {
  throw new Error(LINKING_ERROR);
}

BlobJsiHelper.install();

export function getArrayBuffer(blob: Blob): ArrayBuffer {
  return global.getArrayBufferForBlobId(blob.id);
}
