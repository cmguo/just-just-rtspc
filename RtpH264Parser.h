// RtpH264Parser.h

#ifndef _PPBOX_RTSPC_RTP_H264_PARSER_H_
#define _PPBOX_RTSPC_RTP_H264_PARSER_H_

#include "ppbox/rtspc/RtpParser.h"

#include <ppbox/avcodec/avc/AvcConfigHelper.h>
#include <ppbox/avcodec/nalu/NaluHelper.h>

namespace ppbox
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
                ppbox::demux::StreamInfo & info,
                boost::system::error_code & ec);

            virtual bool push(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool before_seek(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

        private:
            std::vector<boost::uint8_t> buf_; // start_code and nalu headers
            size_t buf_used_;
            std::vector<boost::uint32_t> nalu_sizes_;
            size_t nalu_count_;
            ppbox::avcodec::AvcConfigHelper config_;
            ppbox::avcodec::NaluHelper nalu_;
        };

        PPBOX_REGISTER_RTP_PARSER("H264", RtpH264Parser);

    } // namespace rtspc
} // namespace ppbox

#endif // _PPBOX_RTSPC_RTP_H264_PARSER_H_
