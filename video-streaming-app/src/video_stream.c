#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *demux, *hls_sink;
    GstPad *srcpad, *sinkpad;

    gst_init(&argc, &argv);

    pipeline = gst_pipeline_new("video-stream");
    source = gst_element_factory_make("filesrc", "source");
    g_object_set(source, "location", "/home/sina/Desktop/Project_98101685/video-streaming-app/video.mp4", NULL);
    demux = gst_element_factory_make("qtdemux", "demux");
    hls_sink = gst_element_factory_make("hlssink", "hls_sink");
    g_object_set(hls_sink, "playlist-location", "/home/sina/Desktop/Project_98101685/video-streaming-app/playlist.m3u8", NULL);
    g_object_set(hls_sink, "target-duration", 5, NULL);

    if (!pipeline || !source || !demux || !hls_sink) {
        g_printerr("Failed to create elements.\n");
        return -1;
    }

    gst_bin_add_many(GST_BIN(pipeline), source, demux, hls_sink, NULL);
    if (!gst_element_link(source, demux)) {
        g_printerr("Failed to link source and demux.\n");
        gst_object_unref(pipeline);
        return -1;
    }


    srcpad = gst_element_get_static_pad(source, "src");
    sinkpad = gst_element_get_request_pad(demux, "sink");
    if (!srcpad || !sinkpad) {
        g_printerr("Failed to get pads for source and demux.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    if (gst_pad_link(srcpad, sinkpad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link source and demux pads.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    gst_object_unref(srcpad);
    gst_object_unref(sinkpad);

    if (!gst_element_link(demux, hls_sink)) {
        g_printerr("Failed to link demux and hls_sink.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Parse message */
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;
        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
                g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
                g_error_free(err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End-Of-Stream reached.\n");
                break;
            default:
                break;
        }
        gst_message_unref(msg);
    }

    gst_element_set_state(pipeline, GST_STATE_NULL);

    gst_object_unref(bus);
    gst_object_unref(pipeline);

    return 0;
}

