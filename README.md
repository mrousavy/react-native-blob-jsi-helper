# react-native-blob-jsi-helper

[![Try Showtime!](./img/github-banner.png)](http://showtime.io)

A React Native library for directly accessing an ArrayBuffer of a Blob instance.

> Note: This is a workaround. A long-term solution would be to implement this directly on the Blob instance in React Native Core.

## Installation

```sh
npm install react-native-blob-jsi-helper
```

## Usage

### Accessing the ArrayBuffer of a Blob instance

```js
import { getArrayBufferForBlob } from "react-native-blob-jsi-helper";

// ...

const blob = ...
const arrayBuffer = getArrayBufferForBlob(blob);
```

### Creating a Blob from an ArrayBuffer

```js
import { getBlobForArrayBuffer } from "react-native-blob-jsi-helper";

// ...

const arrayBuffer = ...
const blob = getBlobForArrayBuffer(arrayBuffer);
```

## Performance

If you know me, you know that my libraries are as fast as they can get. react-native-blob-jsi-helper is no different - it is written in C++ JSI and is therefore really fast.

* **Zero** bridge-traffic.
* **Zero** serialization/deserialization.

### Blob -> ArrayBuffer

* For a **10 kB** Buffer, I have measured **1ms**.
* For a **12 MB** Buffer, I have measured **13ms**.

### ArrayBuffer -> Blob

* For a **10 kB** Buffer, I have measured **0.05ms**.
* For a **12 MB** Buffer, I have measured **0.3ms**.

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

MIT
