// RtpFilter.h

#ifndef _PPBOX_RTSPC_RTP_FILTER_H_
#define _PPBOX_RTSPC_RTP_FILTER_H_

#include "ppbox/rtspc/RtpInfo.h"

#include <ppbox/demux/packet/Filter.h>

#include <util/protocol/rtsp/rtp/RtpPacket.h>

#include <framework/system/LimitNumber.h>

namespace ppbox
{
    namespace rtspc
    {

        class RtpParser;

        class RtpFilter
            : public ppbox::demux::Filter
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
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool get_next_sample(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool get_last_sample(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool before_seek(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

        private:
            bool get_sample2(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

            bool parse(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

        private:
            struct RtpSession
            {
                RtpInfo const * rtp_info;
                boost::uint16_t seq_base; // next required sequence
                std::deque<ppbox::demux::Sample> samples; // unordered samples
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
            ppbox::demux::Sample rtcp_sample_;
        };

    } // namespace rtspc
} // namespace ppbox

#endif // _PPBOX_RTSPC_RTP_FILTER_H_
