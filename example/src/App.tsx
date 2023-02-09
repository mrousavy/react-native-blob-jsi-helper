import * as React from 'react';

import { StyleSheet, View } from 'react-native';
import {
  getArrayBufferForBlob,
  getBlobForArrayBuffer,
} from 'react-native-blob-jsi-helper';

export default function App() {
  React.useEffect(() => {
    (async () => {
      console.log('Fetching 8k image..');
      const x = await fetch(
        'https://www.hdwallpapers.in/download/beautiful_lake_landscape_scenery_4k_8k-HD.jpg'
      );
      console.log('Getting blob..');
      const blob = await x.blob();
      console.log(`Blob: ${Object.keys(blob)}..`);
      console.log('Getting ArrayBuffer..');
      // @ts-expect-error performance actually exists.
      // eslint-disable-next-line no-undef
      const start = performance.now();

      const arrayBuffer = getArrayBufferForBlob(blob);
      // @ts-expect-error performance actually exists.
      // eslint-disable-next-line no-undef
      const end = performance.now();
      console.log(
        `Got ArrayBuffer in ${end - start}ms! Size: ${arrayBuffer.byteLength}`
      );

      // @ts-expect-error performance actually exists.
      // eslint-disable-next-line no-undef
      const newStart = performance.now();
      const newBlob = getBlobForArrayBuffer(arrayBuffer.buffer);
      // @ts-expect-error performance actually exists.
      // eslint-disable-next-line no-undef
      const newEnd = performance.now();
      console.log(
        `Converted ArrayBuffer -> Blob in ${newEnd - newStart}ms! Blob ID: ${
          // @ts-expect-error performance actually exists.
          newBlob._data?.blobId
        }`
      );
    })();
  }, []);

  return <View style={styles.container} />;
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  box: {
    width: 60,
    height: 60,
    marginVertical: 20,
  },
});
