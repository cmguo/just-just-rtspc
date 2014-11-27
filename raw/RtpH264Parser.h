// RtpH264Parser.h

#ifndef _JUST_RTSPC_TS_RTP_H264_PARSER_H_
#define _JUST_RTSPC_TS_RTP_H264_PARSER_H_

#include "just/rtspc/RtpParser.h"

#include <just/avcodec/avc/AvcConfigHelper.h>
#include <just/avcodec/nalu/NaluHelper.h>

namespace just
{
    namespace rtspc
    {

        class RtpH264Parser
            : public RtpParser
        {
        public:
            RtpH264Parser();

            virtual ~RtpH264Parser();

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
            std::vector<boost::uint8_t> buf_; // start_code and nalu headers
            size_t buf_used_;
            std::vector<boost::uint32_t> nalu_sizes_;
            size_t nalu_count_;
            just::avcodec::AvcConfigHelper config_;
            just::avcodec::NaluHelper nalu_;
        };

        JUST_REGISTER_RTP_PARSER("H264", RtpH264Parser);

    } // namespace rtspc
} // namespace just

#endif // _JUST_RTSPC_TS_RTP_H264_PARSER_H_
