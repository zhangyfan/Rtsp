#include "Decoder.h"
#include "Decoder_h264.h"

namespace Codec
{

Decoder *Decoder::createNew(const std::string &codecName) {
    if (codecName == "H264") {
        return new DecoderH264();
    }

    return nullptr;
}
}