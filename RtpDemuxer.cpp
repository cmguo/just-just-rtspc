// RtpDemuxer.cpp

#include "just/rtspc/Common.h"
#include "just/rtspc/RtpDemuxer.h"
#include "just/rtspc/RtpFilter.h"
#include "just/rtspc/RtspMedia.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.rtspc.RtpDemuxer", framework::logger::Debug);

namespace just
{
    namespace rtspc
    {

        RtpDemuxer::RtpDemuxer(
            boost::asio::io_service & io_svc, 
            just::data::PacketMedia & media)
            : PacketDemuxer(io_svc, media)
            , filter_(NULL)
        {
        }

        RtpDemuxer::~RtpDemuxer()
        {
            if (filter_) {
                delete filter_;
            }
        }

        void RtpDemuxer::add_parser(
            size_t index, 
            RtpParser * parser)
        {
            filter_->add_parser(
                index, 
                parser);
        }

        bool RtpDemuxer::check_open(
            boost::system::error_code & ec)
        {
            if (filter_ == NULL) {
                filter_ = new RtpFilter;
                add_filter(filter_);
            }

            RtspMedia const & media(static_cast<RtspMedia const &>(PacketDemuxer::media()));

            std::vector<RtpInfo> const & rtp_infos = 
                media.rtsp_source().rtp_infos();

            filter_->set_streams(rtp_infos);

            return true;
        }

    } // namespace rtspc
} // namespace just
