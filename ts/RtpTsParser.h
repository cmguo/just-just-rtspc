// RtpTsParser.h

#ifndef _PPBOX_RTSPC_TS_RTP_TS_PARSER_H_
#define _PPBOX_RTSPC_TS_RTP_TS_PARSER_H_

#include "ppbox/rtspc/RtpParser.h"


namespace ppbox
{
    namespace demux
    {
        class TsDemuxer;
    }

    namespace rtspc
    {

        class RtpTsBuffer;

        class RtpTsParser
            : public RtpParser
        {
        public:
            RtpTsParser(
                boost::asio::io_service & io_svc);

            virtual ~RtpTsParser();

        public:
            void get_streams(
                std::vector<ppbox::demux::StreamInfo> & streams);

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
            RtpTsBuffer * buffer_;
            ppbox::demux::TsDemuxer * demuxer_;
        };

    } // namespace rtspc
} // namespace ppbox

#endif // _PPBOX_RTSPC_TS_RTP_TS_PARSER_H_
