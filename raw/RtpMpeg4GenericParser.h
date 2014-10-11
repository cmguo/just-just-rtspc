// RtpMpeg4GenericParser.h

#ifndef _PPBOX_RTSPC_TS_RTP_MPEG4_GENERIC_PARSER_H_
#define _PPBOX_RTSPC_TS_RTP_MPEG4_GENERIC_PARSER_H_

#include "ppbox/rtspc/RtpParser.h"

#include <ppbox/avcodec/avc/AvcConfigHelper.h>
#include <ppbox/avcodec/nalu/NaluHelper.h>

namespace ppbox
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
                ppbox::demux::StreamInfo & info,
                boost::system::error_code & ec);

            virtual bool push(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool before_seek(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

        private:
            size_t size_length_;
            size_t index_length_;
            size_t index_delta_length_;
        };

        PPBOX_REGISTER_RTP_PARSER("mpeg4-generic", RtpMpeg4GenericParser);

    } // namespace rtspc
} // namespace ppbox

#endif // _PPBOX_RTSPC_TS_RTP_MPEG4_GENERIC_PARSER_H_
