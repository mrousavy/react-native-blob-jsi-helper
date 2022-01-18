# react-native-blob-jsi-helper

[![Try Showtime!](./img/github-banner.png)](http://showtime.io)

A React Native library for directly accessing an ArrayBuffer of a Blob instance.

## Installation

```sh
npm install react-native-blob-jsi-helper
```

## Usage

```js
import { getArrayBuffer } from "react-native-blob-jsi-helper";

// ...

const blob = ...
const arrayBuffer = getArrayBuffer(blob);
```

## Performance

If you know me, you know that my libraries are as fast as they can get. react-native-blob-jsi-helper is no different - it is written in C++ JSI and is therefore really fast.

* **Zero** bridge-traffic.
* **Zero** serialization/deserialization.
* For a **10 kB** Buffer, I have measured **1ms**.
* For a **12 MB** Buffer, I have measured **45ms**.

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

MIT
