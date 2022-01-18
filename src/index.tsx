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

interface BlobData {
  size: number;
  offset: number;
  blobId: string;
}

function isBlobData(data: unknown): data is BlobData {
  return (
    typeof data === 'object' &&
    data != null &&
    'size' in data &&
    'offset' in data &&
    'blobId' in data
  );
}

export function getArrayBuffer(blob: Blob): Uint8Array {
  // @ts-expect-error React Native adds the hidden `_data` field.
  const data = blob._data;
  if (!isBlobData(data))
    throw new Error('Invalid Blob! Blob did not contain a valid ._data field!');

  console.log(`Getting ArrayBuffer for Blob #${data.blobId}...`);
  // @ts-expect-error I inject that function using JSI.
  const buffer = global.getArrayBufferForBlobId(data) as Uint8Array;
  console.log(
    `Got ArrayBuffer for Blob #${data.blobId}! Size: ${buffer?.byteLength}`
  );
  return buffer;
}
