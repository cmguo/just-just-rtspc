// RtpDemuxer.h

#ifndef _PPBOX_RTSPC_RTP_DEMUXER_H_
#define _PPBOX_RTSPC_RTP_DEMUXER_H_

#include "ppbox/demux/packet/PacketDemuxer.h"

#include <ppbox/avformat/flv/FlvMetaData.h>

namespace ppbox
{
    namespace rtspc
    {

        class RtpFilter;

        class RtpDemuxer
            : public ppbox::demux::PacketDemuxer
        {
        public:
            RtpDemuxer(
                boost::asio::io_service & io_svc, 
                ppbox::data::PacketMedia & media);

            virtual ~RtpDemuxer();

        protected:
            virtual bool check_open(
                boost::system::error_code & ec);

        private:
            ppbox::avformat::FlvMetaData meta_;
            RtpFilter * filter_;
        };

        PPBOX_REGISTER_PACKET_DEMUXER("rtp", RtpDemuxer);

    } // namespace rtspc
} // namespace ppbox

#endif // _PPBOX_RTSPC_RTP_DEMUXER_H_
