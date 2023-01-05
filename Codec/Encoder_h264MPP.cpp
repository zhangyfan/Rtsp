#include "Encoder.h"
#include "Encoder_h264.h"

using namespace Codec;

class EncoderH264::impl
{

};

EncoderH264::EncoderH264() {
    impl_ = new impl();
}

EncoderH264::~EncoderH264() {
    if (impl_) {
        delete impl_;
    }
}

bool EncoderH264::encode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength)
{
    return false;
}