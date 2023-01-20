#include "Encoder.h"
#include "Encoder_h264.h"
#include "Encoder_JPEG.h"

namespace Codec
{
Encoder *Encoder::createNew(const std::string &codecName) {
    if (codecName == "H264") {
        return new EncoderH264();
    }
    
    if (codecName == "JPEG") {
        return new EncoderJPEG();
    }

    return nullptr;
}
}