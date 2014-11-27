// RtpMpeg4GenericParser.h

#ifndef _JUST_RTSPC_TS_RTP_MPEG4_GENERIC_PARSER_H_
#define _JUST_RTSPC_TS_RTP_MPEG4_GENERIC_PARSER_H_

#include "just/rtspc/RtpParser.h"

#include <just/avcodec/avc/AvcConfigHelper.h>
#include <just/avcodec/nalu/NaluHelper.h>

namespace just
{
    namespace rtspc
    {

        class RtpMpeg4GenericParser
            : public RtpParser
        {
        public:
            RtpMpeg4GenericParser();

            virtual ~RtpMpeg4GenericParser();

        public:
            virtual bool push(
                just::demux::StreamInfo & info,
                boost::system::error_code & ec);

            virtual bool push(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool before_seek(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

        private:
            size_t size_length_;
            size_t index_length_;
            size_t index_delta_length_;
        };

        JUST_REGISTER_RTP_PARSER("mpeg4-generic", RtpMpeg4GenericParser);

    } // namespace rtspc
} // namespace just

#endif // _JUST_RTSPC_TS_RTP_MPEG4_GENERIC_PARSER_H_
