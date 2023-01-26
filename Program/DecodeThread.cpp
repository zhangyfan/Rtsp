#include "QueuedPacket.h"
#include "Decoder.h"
#include <atomic>
#include <thread>
extern "C" {
#include <libavformat/avformat.h>
}

static std::atomic_bool flag = false;

void DecodeThreadEntry() {
    Codec::Decoder *decoder = Codec::Decoder::createNew("H264");

    while (flag) {
        auto pkt           = getQueuedPacket();
        uint8_t *data      = pkt->data;
        size_t size        = pkt->size;

        auto decodedFrames = decoder->decode(data, size);

        for (auto avFrame : decodedFrames) {
            setQueuedFrame(avFrame);
            av_frame_unref(avFrame);
        }

        av_packet_unref(pkt);
    }
}


void startDecode() {
    flag = true;
    std::thread thread(DecodeThreadEntry);
    thread.detach();
}

void stopDecode() {
    flag = false;
}