#include "Encoder.h"
#include "Encoder_h264.h"

namespace Codec
{
Encoder *Encoder::createNew(const std::string &codecName) {
    if (codecName == "H264") {
        return new EncoderH264();
    }

    return nullptr;
}
}