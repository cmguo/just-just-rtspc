// RtspSource.h

#ifndef _JUST_RTSPC_RTSP_SOURCE_H_
#define _JUST_RTSPC_RTSP_SOURCE_H_

#include "just/rtspc/RtpInfo.h"

#include <just/avbase/MediaInfo.h>
#include <just/avbase/StreamInfo.h>

#include <util/stream/Dual.h>
#include <util/stream/UrlSource.h>

#include <util/protocol/rtsp/RtspSession.h>
#include <util/protocol/rtsp/SessionDescription.h>

namespace just
{
    namespace rtspc
    {

        struct rtsp_source_read_handler;

        using util::protocol::RtspRequest;
        using util::protocol::RtspResponse;

        class RtspSource
            : public util::stream::UrlSource
            , public util::protocol::RtspSession
        {
        public:
            RtspSource(
                boost::asio::io_service & io_svc);

            virtual ~RtspSource();

        public:
            virtual bool open(
                framework::string::Url const & url, 
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec);

            using util::stream::UrlSource::open;

            virtual void async_open(
                framework::string::Url const & url, 
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);

            using util::stream::UrlSource::async_open;

            virtual bool is_open(
                boost::system::error_code & ec);

            virtual bool close(
                boost::system::error_code & ec);

        public:
            virtual bool cancel(
                boost::system::error_code & ec);

        public:
            virtual bool set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            virtual bool set_time_out(
                boost::uint32_t time_out, 
                boost::system::error_code & ec);

            virtual bool continuable(
                boost::system::error_code const & ec);

        public:
            bool is_record() const;

            just::avbase::MediaInfo const & info() const
            {
                return info_;
            }

            util::stream::Dual & rtp_socket()
            {
                return *rtp_socket_;
            }

        public:
           std::vector<RtpInfo> const & rtp_infos() const
           {
               return rtp_infos_;
           }

        private:
            // implement util::stream::Source
            virtual std::size_t private_read_some(
                buffers_t const & buffers,
                boost::system::error_code & ec);

            virtual void private_async_read_some(
                buffers_t const & buffers,
                handler_t const & handler);

        private:
            void handle_connect(
                boost::system::error_code const & ec);

        protected:
            virtual void on_connect();

            // RtspSession
            virtual void on_recv(
                RtspRequest const & req);

            virtual void on_recv(
                RtspResponse const & resp);

            virtual void on_sent(
                RtspRequest const & req);

            virtual void on_sent(
                RtspResponse const & resp);

            virtual void on_error(
                boost::system::error_code const & ec);

        private:
            void response(
                boost::system::error_code const & ec);

            void parse_sdp(
                boost::asio::streambuf const & data, 
                boost::system::error_code & ec);

        protected:
            util::stream::Dual * rtp_socket_;
            util::protocol::RtspRequest request_;
            util::protocol::SessionDescription sdp_;
            just::avbase::MediaInfo info_;
            std::vector<RtpInfo> rtp_infos_;
            std::string content_base_;
            size_t setup_step_;

        private:
            response_type resp_;
        };

        UTIL_REGISTER_URL_SOURCE("rtsp", RtspSource);

        struct RtspSourceTraits
            : util::tools::ClassFactoryTraits
        {
            typedef std::string key_type;
            typedef RtspSource * (create_proto)(
                boost::asio::io_service &);

            static boost::system::error_code error_not_found();
        };

        typedef util::tools::ClassFactory<RtspSourceTraits> RtspSourceFactory;

    } // namespace data
} // namespace just

#define JUST_REGISTER_RTSP_SOURCE(k, c) UTIL_REGISTER_CLASS(just::rtspc::RtspSourceFactory, k, c)

#endif // _JUST_RTSPC_RTSP_SOURCE_H_
