// RtpTsParser.h

#ifndef _JUST_RTSPC_TS_RTP_TS_PARSER_H_
#define _JUST_RTSPC_TS_RTP_TS_PARSER_H_

#include "just/rtspc/RtpParser.h"


namespace just
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
                std::vector<just::demux::StreamInfo> & streams);

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
            RtpTsBuffer * buffer_;
            just::demux::TsDemuxer * demuxer_;
        };

    } // namespace rtspc
} // namespace just

#endif // _JUST_RTSPC_TS_RTP_TS_PARSER_H_
