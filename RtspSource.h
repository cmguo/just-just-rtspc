// RtspSource.h

#ifndef _PPBOX_RTSPC_RTSP_SOURCE_H_
#define _PPBOX_RTSPC_RTSP_SOURCE_H_

#include <ppbox/avbase/MediaInfo.h>
#include <ppbox/avbase/StreamInfo.h>

#include <util/stream/Dual.h>
#include <util/stream/UrlSource.h>

#include <util/protocol/rtsp/RtspSession.h>
#include <util/protocol/rtsp/SessionDescription.h>

namespace ppbox
{
    namespace rtspc
    {

        struct rtsp_source_read_handler;

        using util::protocol::RtspRequest;
        using util::protocol::RtspResponse;

        struct RtpInfo
        {
            std::string type;
            int format;
            std::string codec;
            int clock;
            std::string param;
            std::string fparam;
            std::string control;
            boost::uint32_t ssrc;
            boost::uint32_t sequence;
            boost::uint32_t timestamp;
        };

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

            ppbox::avbase::MediaInfo const & info() const
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

            // RtspSession
            virtual void on_recv(
                RtspRequest const & req);

            virtual void on_recv(
                RtspResponse const & resp);

            virtual void on_error(
                boost::system::error_code const & ec);

        private:
            void response(
                boost::system::error_code const & ec);

            void parse_sdp(
                boost::asio::streambuf const & data, 
                boost::system::error_code & ec);

        private:
            response_type resp_;
            util::protocol::RtspRequest request_;
            util::protocol::SessionDescription sdp_;
            ppbox::avbase::MediaInfo info_;
            //std::vector<ppbox::avbase::StreamInfo> streams_;
            std::vector<RtpInfo> rtp_infos_;
            std::string content_base_;
            util::stream::Dual * rtp_socket_;
            size_t setup_step_;
        };

        UTIL_REGISTER_URL_SOURCE("rtsp", RtspSource);

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_RTSPC_RTSP_SOURCE_H_
