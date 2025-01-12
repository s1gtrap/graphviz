Building for native

```sh
./autogen.sh
./configure
make
./dotc.sh
```

Trying to build for wasm with emscripten

```sh
./autogen.sh
emconfigure ./configure
emmake make
./dotc.sh
```
