// RtpRawDemuxer.h

#ifndef _JUST_RTSPC_RAW_RTP_RAW_DEMUXER_H_
#define _JUST_RTSPC_RAW_RTP_RAW_DEMUXER_H_

#include "just/rtspc/RtpDemuxer.h"

namespace just
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
                just::data::PacketMedia & media);

            virtual ~RtpRawDemuxer();

        protected:
            virtual bool check_open(
                boost::system::error_code & ec);
        };

        JUST_REGISTER_PACKET_DEMUXER("rtp", RtpRawDemuxer);

    } // namespace rtspc
} // namespace just

#endif // _JUST_RTSPC_RAW_RTP_RAW_DEMUXER_H_
