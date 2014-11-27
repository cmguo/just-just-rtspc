// RtpDemuxer.h

#ifndef _JUST_RTSPC_RTP_DEMUXER_H_
#define _JUST_RTSPC_RTP_DEMUXER_H_

#include "just/demux/packet/PacketDemuxer.h"

namespace just
{
    namespace rtspc
    {

        class RtpFilter;
        class RtpParser;

        class RtpDemuxer
            : public just::demux::PacketDemuxer
        {
        public:
            RtpDemuxer(
                boost::asio::io_service & io_svc, 
                just::data::PacketMedia & media);

            virtual ~RtpDemuxer();

        protected:
            void add_parser(
                size_t index, 
                RtpParser * parser);

        protected:
            virtual bool check_open(
                boost::system::error_code & ec);

        private:
            RtpFilter * filter_;
            std::vector<RtpParser *> parsers_;
        };

    } // namespace rtspc
} // namespace just

#endif // _JUST_RTSPC_RTP_DEMUXER_H_
