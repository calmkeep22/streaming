# streaming

GStreamer(C)로 만든 RTP/H.264 UDP 영상 스트리밍 예제입니다.

## 구성

```
streaming.sln
├─ sender/     sender.c   : videotestsrc → videoconvert → x264enc → rtph264pay → udpsink (127.0.0.1:5000)
└─ receiver/   receiver.c : udpsrc(:5000) → rtph264depay → H.264 decoder → videoconvert → autovideosink
```

## 빌드 (Windows / Visual Studio)

1. [GStreamer](https://gstreamer.freedesktop.org/download/) MSVC 64-bit **runtime + development** 설치
2. `streaming.sln`을 열고 **x64** 구성으로 빌드 (sender, receiver 두 프로젝트가 함께 빌드됨)

GStreamer 경로는 설치 시 등록되는 환경 변수 `GSTREAMER_1_0_ROOT_MSVC_X86_64`를 사용하고,
없으면 `C:\Program Files\gstreamer.0\msvc_x86_64\`를 사용합니다.
VS에서 디버그 실행할 때는 GStreamer `bin` 폴더가 `PATH`에 자동으로 추가됩니다.

## 실행

```
receiver.exe   # 먼저 수신 측 실행
sender.exe     # 다른 터미널에서 송신 측 실행
```
