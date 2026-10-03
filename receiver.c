#include <gst/gst.h>

int main(int argc, char* argv[]) {

    GstElement* pipeline;
    GstElement* source;
    GstElement* depayloader;
    GstElement* decoder;
    GstElement* convert;
    GstElement* sink;

    GstBus* bus;
    GstMessage* msg;

    GstCaps* caps;

    // 1. GStreamer 초기화
    gst_init(&argc, &argv);

    // 2. Pipeline 생성
    pipeline = gst_pipeline_new("udp-receiver");

    // 3. Element 생성
    source =
        gst_element_factory_make(
            "udpsrc",
            "source"
        );

    depayloader =
        gst_element_factory_make(
            "rtph264depay",
            "depayloader"
        );

    decoder =
        gst_element_factory_make(
            "avdec_h264",
            "decoder"
        );

    convert =
        gst_element_factory_make(
            "videoconvert",
            "convert"
        );

    sink =
        gst_element_factory_make(
            "autovideosink",
            "sink"
        );

    if (!pipeline ||
        !source ||
        !depayloader ||
        !decoder ||
        !convert ||
        !sink) {

        g_printerr("Element 생성 실패\n");
        return -1;
    }

    // 4. RTP Caps 생성
    caps = gst_caps_from_string(
        "application/x-rtp,"
        "media=video,"
        "encoding-name=H264,"
        "payload=96"
    );

    // 5. UDP source 설정
    g_object_set(
        source,
        "port", 5000,
        "caps", caps,
        NULL
    );

    gst_caps_unref(caps);

    // 6. Pipeline에 Element 추가
    gst_bin_add_many(
        GST_BIN(pipeline),
        source,
        depayloader,
        decoder,
        convert,
        sink,
        NULL
    );

    // 7. Element 연결
    if (!gst_element_link_many(
        source,
        depayloader,
        decoder,
        convert,
        sink,
        NULL)) {

        g_printerr("Element 연결 실패\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // 8. Pipeline 실행
    GstStateChangeReturn ret =
        gst_element_set_state(
            pipeline,
            GST_STATE_PLAYING
        );

    if (ret == GST_STATE_CHANGE_FAILURE) {

        g_printerr("Pipeline 실행 실패\n");

        gst_object_unref(pipeline);

        return -1;
    }

    g_print("UDP Receiver 시작\n");
    g_print("Port: 5000\n");

    // 9. Bus
    bus = gst_element_get_bus(pipeline);

    msg = gst_bus_timed_pop_filtered(
        bus,
        GST_CLOCK_TIME_NONE,
        GST_MESSAGE_ERROR |
        GST_MESSAGE_EOS
    );

    // 10. Message 처리
    if (msg != NULL) {

        switch (GST_MESSAGE_TYPE(msg)) {

        case GST_MESSAGE_ERROR: {

            GError* err = NULL;
            gchar* debug_info = NULL;

            gst_message_parse_error(
                msg,
                &err,
                &debug_info
            );

            g_printerr(
                "Error: %s\n",
                err->message
            );

            if (debug_info != NULL) {
                g_printerr(
                    "Debug: %s\n",
                    debug_info
                );
            }

            g_clear_error(&err);
            g_free(debug_info);

            break;
        }

        case GST_MESSAGE_EOS:

            g_print("EOS\n");

            break;

        default:
            break;
        }

        gst_message_unref(msg);
    }

    // 11. 종료
    gst_element_set_state(
        pipeline,
        GST_STATE_NULL
    );

    gst_object_unref(bus);
    gst_object_unref(pipeline);

    return 0;
}