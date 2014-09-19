// RtspMedia.cpp

#include "ppbox/rtspc/Common.h"
#include "ppbox/rtspc/RtspMedia.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/string/Algorithm.h>

#include <boost/bind.hpp>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.rtspc.RtspMedia", framework::logger::Debug);

namespace ppbox
{
    namespace rtspc
    {

        RtspMedia::RtspMedia(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : PacketMedia(io_svc, url)
            , source_(new RtspSource(io_svc))
        {
            boost::system::error_code ec1;
            PacketMedia::get_basic_info(info_, ec1);
            info_.flags |= ppbox::data::PacketMediaFlags::f_non_ordered;
            info_.format = "rtp";
        }

        RtspMedia::~RtspMedia()
        {
            // auto delete
            source_ = NULL;
        }

        void RtspMedia::async_open(
            MediaBase::response_type const & resp)
        {
            source_->async_open(url_, 
                boost::bind(&RtspMedia::handle_open, this, _1, resp));
        }

        void RtspMedia::handle_open(
            boost::system::error_code const & ec, 
            MediaBase::response_type const & resp)
        {
            info_ = source_->info();
            boost::system::error_code ec1;
            PacketMedia::get_basic_info(info_, ec1);
            info_.flags |= ppbox::data::PacketMediaFlags::f_non_ordered;
            info_.format = "rtp";
            if (!ec && source_->is_record()) {
                info_.type = info_.vod;
                info_.flags |= info_.f_seekable;
                info_.flags |= info_.f_pauseable;
            }
            resp(ec);
        }

        void RtspMedia::cancel(
            boost::system::error_code & ec)
        {
            source_->cancel(ec);
        }

        void RtspMedia::close(
            boost::system::error_code & ec)
        {
            source_->close(ec);
        }

        bool RtspMedia::get_basic_info(
            ppbox::data::MediaBasicInfo & info,
            boost::system::error_code & ec) const
        {
            info = info_;
            ec.clear();
            return true;
        }

        bool RtspMedia::get_info(
            ppbox::data::MediaInfo & info,
            boost::system::error_code & ec) const
        {
            info = info_;
            ec.clear();
            return PacketMedia::get_info(info, ec);
        }

        bool RtspMedia::get_packet_feature(
            ppbox::data::PacketFeature & feature,
            boost::system::error_code & ec) const
        {
            feature.piece_size = 1024;
            feature.packet_max_size = 1024 * 200;
            feature.buffer_size = 1024 * 1024 * 2; // 2M
            feature.prepare_size = 1024 * 10;
            ec.clear();
            return true;
        }

        util::stream::Source & RtspMedia::source()
        {
            return *source_;
        }

    } // rtspc
} // ppbox
