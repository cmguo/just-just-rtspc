// RtpTsDemuxer.h

#ifndef _PPBOX_RTSPC_TS_RTP_TS_DEMUXER_H_
#define _PPBOX_RTSPC_TS_RTP_TS_DEMUXER_H_

#include "ppbox/rtspc/RtpDemuxer.h"

namespace ppbox
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
                ppbox::data::PacketMedia & media);

            virtual ~RtpTsDemuxer();

        protected:
            virtual bool check_open(
                boost::system::error_code & ec);

        private:
            RtpTsParser * ts_parser_;
        };

        PPBOX_REGISTER_PACKET_DEMUXER("rtp-ts", RtpTsDemuxer);

    } // namespace rtspc
} // namespace ppbox

#endif // _PPBOX_RTSPC_TS_RTP_TS_DEMUXER_H_
