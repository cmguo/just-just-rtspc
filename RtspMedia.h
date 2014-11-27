// RtspMedia.h

#ifndef _JUST_RTSPC_RTSP_MEDIA_H_
#define _JUST_RTSPC_RTSP_MEDIA_H_

#include "just/rtspc/RtspSource.h"

#include <just/data/packet/PacketMedia.h>

namespace just
{
    namespace rtspc
    {

        class RtspMedia
            : public just::data::PacketMedia
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
                just::avbase::MediaBasicInfo & info,
                boost::system::error_code & ec) const;

            virtual bool get_info(
                just::avbase::MediaInfo & info,
                boost::system::error_code & ec) const;

        public:
            virtual bool get_packet_feature(
                just::data::PacketFeature & feature,
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
            just::avbase::MediaInfo info_;
        };

        JUST_REGISTER_MEDIA_BY_PROTOCOL("rtsp", RtspMedia);

    } // data
} // just

#endif // _JUST_RTSPC_RTSP_MEDIA_H_
