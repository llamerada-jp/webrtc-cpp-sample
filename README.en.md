# WebRTC C++ sample
Sample program for using WebRTC(DataChannel) on C++.

# Requirement

* Mac OSX
* WebRTC(require Debug build before compile this project)(https://webrtc.org/native-code/development/).

# Compile

```sh
$ mkdir -p <path to work>/libs
$ cp <path to webrtc>/src/out/Release/lib* <path to work>/libs
$ cd <path to work>/libs
$ rm *test*
$ find *.a -exec ar x {} \;
$ ar q libwebrtc_all.a *.o
$ cd <path to work>
$ git clone --depth 1 https://github.com/llamerada-jp/webrtc-cpp-sample.git
$ cd webrtc-cpp-sample
$ git submodule init
$ git submodule update
$ cd <path to work>
$ clang++ -std=c++11 -I <path to webrtc>/src/ -L <path to work>/libs -L <path to webrtc>/src/out/Debug/ -lwebrtc_all -lwebrtc -framework CoreFoundation -framework Foundation -framework CoreAudio -framework AudioToolbox -framework CoreGraphics -o sample <path to work>/webrtc-cpp-sample/main.cpp
```

# Run

This sample use two consoles to try interprocess communication by WebRTC.
It maybe cannot communicate over NAT each other, because it does not use ICE server.

## Connection

memo : On this sample, Some commands requireing parameter need line of only a semicolon after parameter.

At CONSOLE-1.

```sh
$ cd <path to work>
$ ./sample
0x7fff791c9000:Main thread
0x700000081000:RTC thread
sdp1
0x700000081000:PeerConnectionObserver::RenegotiationNeeded
0x700000081000:CreateSessionDescriptionObserver::OnSuccess
0x700000081000:PeerConnectionObserver::SignalingChange(1)
Offer SDP:begin
<Copy displayed string to the clipboard as STRING-A.>
Offer SDP:end
0x700000081000:SetSessionDescriptionObserver::OnSuccess
0x700000081000:PeerConnectionObserver::IceGatheringChange(1)
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceGatheringChange(2)
sdp3
<Paste STRING-B that it displayed on CONSOLE-2.>
;
0x700000081000:PeerConnectionObserver::SignalingChange(0)
0x700000081000:PeerConnectionObserver::IceConnectionChange(1)
0x700000081000:SetSessionDescriptionObserver::OnSuccess
ice1
<Copy displayed string to the clipboard as STRING-C.>
0x700000081000:PeerConnectionObserver::IceConnectionChange(2)
0x700000081000:PeerConnectionObserver::IceConnectionChange(3)
0x700000081000:DataChannelObserver::StateChange
0x700000081000:PeerConnectionObserver::DataChannel(0x7fd8cb608750, 0x7fd8cb71bef0)
ice2
<Paste STRING-D that it displayed on CONSOLE-2.>
;
```

At CONSOLE-2.

```sh
$ cd <path to work>
$ ./sample
0x7fff791c9000:Main thread
0x700000081000:RTC thread
sdp2
<Paste STRING-A that it displayed on CONSOLE-1.>
;
0x700000081000:PeerConnectionObserver::RenegotiationNeeded
0x700000081000:PeerConnectionObserver::SignalingChange(3)
0x700000081000:SetSessionDescriptionObserver::OnSuccess
0x700000081000:CreateSessionDescriptionObserver::OnSuccess
0x700000081000:PeerConnectionObserver::SignalingChange(0)
Answer SDP:begin
<Copy displayed string to the clipboard as STRING-B.>
Answer SDP:end
0x700000081000:SetSessionDescriptionObserver::OnSuccess
0x700000081000:PeerConnectionObserver::IceGatheringChange(1)
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceGatheringChange(2)
ice2
<Paste STRING-C that it displayed on CONSOLE-1.>
;
0x700000081000:PeerConnectionObserver::IceConnectionChange(1)
0x700000081000:PeerConnectionObserver::IceConnectionChange(2)
0x700000081000:DataChannelObserver::StateChange
0x700000081000:PeerConnectionObserver::DataChannel(0x7fa739e0c0d0, 0x7fa739e08b80)
ice1
<Copy displayed string to the clipboard as STRING-D.>
```

## Send message

You can send messages, after connection is enabled.

```
send
Hello world.
;
```

## Quit

You can watch sequence of quit by typing of "quit".

```
quit
```

EOD
