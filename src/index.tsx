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

export function getArrayBufferForBlob(blob: Blob): Uint8Array {
  // @ts-expect-error React Native adds the hidden `_data` field.
  const data = blob._data;
  if (!isBlobData(data))
    throw new Error('Invalid Blob! Blob did not contain a valid ._data field!');

  // @ts-expect-error I inject that function using JSI.
  const buffer = global.getArrayBufferForBlob(data) as Uint8Array;
  return buffer;
}

export function getBlobForArrayBuffer(arrayBuffer: ArrayBuffer): Blob {
  if (!(arrayBuffer instanceof ArrayBuffer)) {
    throw new Error('arrayBuffer is not instance of ArrayBuffer!');
  }
  if (arrayBuffer.byteLength < 1)
    throw new Error(
      'Invalid ArrayBuffer! ArrayBuffer.byteLength has to be greater than 0!'
    );

  // @ts-expect-error I inject that function using JSI.
  const blobData = global.getBlobForArrayBuffer(arrayBuffer) as Blob;
  const data = { ...blobData, type: 'blob' };

  return {
    // @ts-expect-error Blob is actually differently typed
    data: data,
    _data: data,
    type: 'blob',
  };
}
