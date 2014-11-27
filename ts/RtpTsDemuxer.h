// RtpTsDemuxer.h

#ifndef _JUST_RTSPC_TS_RTP_TS_DEMUXER_H_
#define _JUST_RTSPC_TS_RTP_TS_DEMUXER_H_

#include "just/rtspc/RtpDemuxer.h"

namespace just
{
    namespace rtspc
    {

        class RtpTsParser;

        class RtpTsDemuxer
            : public RtpDemuxer
        {
        public:
            RtpTsDemuxer(
                boost::asio::io_service & io_svc, 
                just::data::PacketMedia & media);

            virtual ~RtpTsDemuxer();

        protected:
            virtual bool check_open(
                boost::system::error_code & ec);

        private:
            RtpTsParser * ts_parser_;
        };

        JUST_REGISTER_PACKET_DEMUXER("rtp-ts", RtpTsDemuxer);

    } // namespace rtspc
} // namespace just

#endif // _JUST_RTSPC_TS_RTP_TS_DEMUXER_H_
