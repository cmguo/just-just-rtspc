// RtspMedia.h

#ifndef _PPBOX_RTSPC_RTSP_MEDIA_H_
#define _PPBOX_RTSPC_RTSP_MEDIA_H_

#include "ppbox/rtspc/RtspSource.h"

#include <ppbox/data/packet/PacketMedia.h>

namespace ppbox
{
    namespace rtspc
    {

        class RtspMedia
            : public ppbox::data::PacketMedia
        {
        public:
            RtspMedia(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

            virtual ~RtspMedia();

        public:
            virtual void async_open(
                response_type const & resp);

            virtual void cancel(
                boost::system::error_code & ec);

            virtual void close(
                boost::system::error_code & ec);

        public:
            virtual bool get_basic_info(
                ppbox::avbase::MediaBasicInfo & info,
                boost::system::error_code & ec) const;

            virtual bool get_info(
                ppbox::avbase::MediaInfo & info,
                boost::system::error_code & ec) const;

        public:
            virtual bool get_packet_feature(
                ppbox::data::PacketFeature & feature,
                boost::system::error_code & ec) const;

            virtual util::stream::Source & source();

        public:
            RtspSource const & rtsp_source() const
            {
                return *source_;
            }

        private:
            void handle_open(
                boost::system::error_code const & ec, 
                MediaBase::response_type const & resp);

        private:
            RtspSource * source_;
            ppbox::avbase::MediaInfo info_;
        };

        PPBOX_REGISTER_MEDIA_BY_PROTOCOL("rtsp", RtspMedia);

    } // data
} // ppbox

#endif // _PPBOX_RTSPC_RTSP_MEDIA_H_
