#include "Decoder_h264.h"

using namespace Codec;

class DecoderH264::impl
{

};

DecoderH264::DecoderH264() {
    impl_ = new impl();
}

DecoderH264::~DecoderH264() {
    if (impl_) {
        delete impl_;
    }
}

bool DecoderH264::decode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength)
{
    return false;
}