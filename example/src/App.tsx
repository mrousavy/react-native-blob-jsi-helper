import * as React from 'react';

import { StyleSheet, View, Text } from 'react-native';
import { getArrayBuffer } from 'react-native-blob-jsi-helper';

export default function App() {
  const [result, setResult] = React.useState<number | undefined>();

  React.useEffect(() => {
    (async () => {
      console.log('Fetching google..');
      const x = await fetch('https://google.com');
      console.log('Getting blob..');
      const blob = await x.blob();
      console.log(`Blob: ${Object.keys(blob)}..`);
      console.log('Getting ArrayBuffer..');
      const arrayBuffer = getArrayBuffer(blob);
      console.log(`ArrayBuffer: ${arrayBuffer.byteLength}..`);
    })();
  }, []);

  return (
    <View style={styles.container}>
      <Text>Result: {result}</Text>
    </View>
  );
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
