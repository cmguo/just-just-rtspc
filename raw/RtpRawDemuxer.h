// RtpRawDemuxer.h

#ifndef _PPBOX_RTSPC_RAW_RTP_RAW_DEMUXER_H_
#define _PPBOX_RTSPC_RAW_RTP_RAW_DEMUXER_H_

#include "ppbox/rtspc/RtpDemuxer.h"

namespace ppbox
{
    namespace rtspc
    {

        class RtpParser;

        class RtpRawDemuxer
            : public RtpDemuxer
        {
        public:
            RtpRawDemuxer(
                boost::asio::io_service & io_svc, 
                ppbox::data::PacketMedia & media);

            virtual ~RtpRawDemuxer();

        protected:
            virtual bool check_open(
                boost::system::error_code & ec);
        };

        PPBOX_REGISTER_PACKET_DEMUXER("rtp", RtpRawDemuxer);

    } // namespace rtspc
} // namespace ppbox

#endif // _PPBOX_RTSPC_RAW_RTP_RAW_DEMUXER_H_
