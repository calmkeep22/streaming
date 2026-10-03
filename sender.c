#include <gst/gst.h>

int main(int argc, char* argv[]) {
	GstElement* pipeline;
	GstElement* source;
	GstElement* convert;
	GstElement* encoder;
	GstElement* payloader;
	GstElement* sink;

	GstBus* bus;
	GstMessage* msg;

	gst_init(&argc, &argv);
	pipeline = gst_pipeline_new("udp-sender");

	source = gst_element_factory_make("videotestsrc", "source");
	convert = gst_element_factory_make("videoconvert", "convert");
	encoder = gst_element_factory_make("x264enc", "encoder");
	payloader = gst_element_factory_make("rtph264pay", "payloader");
	sink = gst_element_factory_make("udpsink", "sink");
	if (!pipeline ||
		!source ||
		!convert ||
		!encoder ||
		!payloader ||
		!sink) {

		g_printerr("Element 생성 실패\n");
		return -1;
	}

    // 4. source 설정
    g_object_set(
        source,
        "is-live", TRUE,
        NULL
    );

    // 5. Encoder 설정
    g_object_set(
        encoder,
        "tune", 0x00000004,  // zerolatency
        "bitrate", 1000,
        "speed-preset", 1,
        NULL
    );

    // 6. RTP 설정
    g_object_set(
        payloader,
        "pt", 96,
        "config-interval", 1,
        NULL
    );

    // 7. UDP 목적지 설정
    g_object_set(
        sink,
        "host", "127.0.0.1",
        "port", 5000,
        NULL
    );

    // 8. Pipeline에 element 등록
    gst_bin_add_many(
        GST_BIN(pipeline),
        source,
        convert,
        encoder,
        payloader,
        sink,
        NULL
    );

    // 9. Element 연결
    if (!gst_element_link_many(
        source,
        convert,
        encoder,
        payloader,
        sink,
        NULL)) {

        g_printerr("Element 연결 실패\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // 10. Pipeline 실행
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

    g_print("UDP Streaming 시작\n");
    g_print("127.0.0.1:5000\n");

    // 11. Bus 가져오기
    bus = gst_element_get_bus(pipeline);

    // ERROR 또는 EOS가 올 때까지 대기
    msg = gst_bus_timed_pop_filtered(
        bus,
        GST_CLOCK_TIME_NONE,
        GST_MESSAGE_ERROR |
        GST_MESSAGE_EOS
    );

    // 12. Message 처리
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

    // 13. 종료
    gst_element_set_state(
        pipeline,
        GST_STATE_NULL
    );

    gst_object_unref(bus);
    gst_object_unref(pipeline);

    return 0;
}