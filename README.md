# streaming

GStreamer(C)로 만든 RTP/H.264 UDP 영상 스트리밍 예제입니다.

## 구성

| 파일 | 설명 |
|------|------|
| `sender.c` | `videotestsrc → videoconvert → x264enc → rtph264pay → udpsink` (127.0.0.1:5000으로 송신) |
| `receiver.c` | `udpsrc(:5000) → rtph264depay → H.264 decoder → videoconvert → autovideosink` |
| `Gstreamer.props` | GStreamer include/lib 경로를 담은 Visual Studio 속성 시트 |

## 빌드 (Windows / Visual Studio)

1. [GStreamer](https://gstreamer.freedesktop.org/download/) MSVC 64-bit **runtime + development** 설치
   (기본 경로 `C:\Program Files\gstreamer\1.0\msvc_x86_64`)
2. 설치 경로가 다르면 `Gstreamer.props`의 경로 수정
3. `Project1.sln`을 열고 **x64** 구성으로 빌드
4. `C:\Program Files\gstreamer\1.0\msvc_x86_64\bin`을 `PATH`에 추가

현재 프로젝트에는 `receiver.c`만 포함되어 있습니다. 송신 측을 빌드하려면 프로젝트에서 `receiver.c`를 빼고 `sender.c`를 추가하세요.

## 실행

```
receiver.exe   # 먼저 수신 측 실행
sender.exe     # 다른 터미널에서 송신 측 실행
```
