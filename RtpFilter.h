// RtpFilter.h

#ifndef _JUST_RTSPC_RTP_FILTER_H_
#define _JUST_RTSPC_RTP_FILTER_H_

#include "just/rtspc/RtpInfo.h"

#include <just/demux/packet/Filter.h>

#include <util/protocol/rtsp/rtp/RtpPacket.h>

#include <framework/system/LimitNumber.h>

namespace just
{
    namespace rtspc
    {

        class RtpParser;

        class RtpFilter
            : public just::demux::Filter
        {
        public:
            RtpFilter();

            ~RtpFilter();

        public:
            void set_streams(
                std::vector<RtpInfo> const & rtp_infos);

            void add_parser(
                size_t index, 
                RtpParser * parser);

        public:
            virtual bool get_sample(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool get_next_sample(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool get_last_sample(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool before_seek(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

        private:
            bool get_sample2(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

            bool parse(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

        private:
            struct RtpSession
            {
                RtpInfo const * rtp_info;
                boost::uint16_t seq_base; // next required sequence
                std::deque<just::demux::Sample> samples; // unordered samples
                framework::system::LimitNumber<32> timestamp_;
                RtpParser * parser;

                RtpSession(
                    RtpInfo const & rtp_info)
                    : rtp_info(&rtp_info)
                    , seq_base(rtp_info.sequence)
                    , parser(NULL)
                {
                }

                bool ready() const
                {
                    return !(samples.empty() || samples.front().memory == NULL);
                }
            };
            std::vector<RtpSession> sessions_;
            RtpSession * ready_session_;
            util::protocol::RtpHead rtp_head_;
            just::demux::Sample rtcp_sample_;
        };

    } // namespace rtspc
} // namespace just

#endif // _JUST_RTSPC_RTP_FILTER_H_
