# WebRTC C++ sample
Sample program for using WebRTC(DataChannel) on C++.
(README.en.md is English translation of this file.)

WebRTCのDataChannelをC++から利用するサンプルコード。

# Requirement

* Mac OSX
* Ubuntu

# Compile

```sh
$ cd <path to work>
$ git clone --depth 1 https://github.com/llamerada-jp/webrtc-cpp-sample.git
$ cd webrtc-cpp-sample
$ git submodule init
$ git submodule update
$ sh build.sh
```

# Run

コンソールを２つ起動して、プロセス間通信ができることを確認。
ICEサーバは使っていないので、NAT同士の通信はできないはず。

## Connection

コンソール1で作業

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
<文字列Aとしてコピー>
Offer SDP:end
0x700000081000:SetSessionDescriptionObserver::OnSuccess
0x700000081000:PeerConnectionObserver::IceGatheringChange(1)
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceGatheringChange(2)
sdp3
<コンソール2に表示される文字列Bを貼り付け>
;
0x700000081000:PeerConnectionObserver::SignalingChange(0)
0x700000081000:PeerConnectionObserver::IceConnectionChange(1)
0x700000081000:SetSessionDescriptionObserver::OnSuccess
ice1
<文字列Cとしてコピー>
0x700000081000:PeerConnectionObserver::IceConnectionChange(2)
0x700000081000:PeerConnectionObserver::IceConnectionChange(3)
0x700000081000:DataChannelObserver::StateChange
0x700000081000:PeerConnectionObserver::DataChannel(0x7fd8cb608750, 0x7fd8cb71bef0)
ice2
<コンソール2に表示される文字列Dを貼り付け>
;
```

コンソール2で作業

```sh
$ cd <path to work>
$ ./sample
0x7fff791c9000:Main thread
0x700000081000:RTC thread
sdp2
<コンソール1に表示される文字列Aを貼り付け>
;
0x700000081000:PeerConnectionObserver::RenegotiationNeeded
0x700000081000:PeerConnectionObserver::SignalingChange(3)
0x700000081000:SetSessionDescriptionObserver::OnSuccess
0x700000081000:CreateSessionDescriptionObserver::OnSuccess
0x700000081000:PeerConnectionObserver::SignalingChange(0)
Answer SDP:begin
<文字列Bとしてコピー>
Answer SDP:end
0x700000081000:SetSessionDescriptionObserver::OnSuccess
0x700000081000:PeerConnectionObserver::IceGatheringChange(1)
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceCandidate
0x700000081000:PeerConnectionObserver::IceGatheringChange(2)
ice2
<コンソール1に表示される文字列Cを貼り付け>
;
0x700000081000:PeerConnectionObserver::IceConnectionChange(1)
0x700000081000:PeerConnectionObserver::IceConnectionChange(2)
0x700000081000:DataChannelObserver::StateChange
0x700000081000:PeerConnectionObserver::DataChannel(0x7fa739e0c0d0, 0x7fa739e08b80)
ice1
<文字列Dとしてコピー>
```

## Send message

接続が完了したらメッセージの送受信ができる。

```
send
Hello world.
;
```

## Quit

quitして接続状況の変化が観察できる。

```
quit
```

以上
