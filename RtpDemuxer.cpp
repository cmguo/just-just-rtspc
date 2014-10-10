// RtpDemuxer.cpp

#include "ppbox/rtspc/Common.h"
#include "ppbox/rtspc/RtpDemuxer.h"
#include "ppbox/rtspc/RtpFilter.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.rtspc.RtpDemuxer", framework::logger::Debug);

namespace ppbox
{
    namespace rtspc
    {

        RtpDemuxer::RtpDemuxer(
            boost::asio::io_service & io_svc, 
            ppbox::data::PacketMedia & media)
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
            filter_->add_stream(
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

            return true;
        }

    } // namespace rtspc
} // namespace ppbox
