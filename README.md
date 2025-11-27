# WebRTC Example

WebRTC example demonstrating interaction between browser WebRTC and [libwebrtc](https://webrtc.googlesource.com/src):
- `server`: Handles signaling between `client` and `app`.
- `client`: Captures and sends video frames.
- `app`: Receives, transforms and sends back video frames.

Tested on:
- [ ] Linux
- [ ] Mac
- [x] Windows

## Build

Intall server.
```
cd server
npm install
```

Intall client.
```
cd client
npm install
```

Build WebRTC.
```
cd app
python ./scripts/build_webrtc.py run --branch "branch-heads/7204"
```
* Output: `./external/webrtc/`

Build app.
```
cd app
python ./scripts/build.py
```
* Output: `./build/Release/webrtc-example.exe`

## Run

Run server.
```
cd server
npm start
```

Run client.
```
cd client
npm start
```
* http://127.0.0.1:8000/

Run app.
```
cd app
./build/Release/webrtc-example.exe
```
