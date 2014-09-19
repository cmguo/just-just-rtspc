// RtspSource.cpp

#include "ppbox/rtspc/Common.h"
#include "ppbox/rtspc/RtspSource.h"

#include <ppbox/avbase/StreamType.h>

#include <util/protocol/rtsp/RtspError.h>
#include <util/protocol/rtsp/RtspSocket.hpp>
#include <util/protocol/rtsp/RtspTransport.h>
using namespace util::protocol;
#include <util/archive/ArchiveBuffer.h>

#include <boost/asio/buffer.hpp>

#include <framework/network/NetName.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/StringRecord.h>

namespace ppbox
{
    namespace rtspc
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.rtspc.RtspSource", framework::logger::Debug);

        RtspSource::RtspSource(
            boost::asio::io_service & io_svc)
            : util::stream::UrlSource(io_svc)
            , RtspSession(io_svc)
            , rtp_socket_(NULL)
            , setup_step_(0)
        {
        }

        RtspSource::~RtspSource()
        {
        }

        bool RtspSource::open(
            framework::string::Url const & url, 
            boost::uint64_t beg, 
            boost::uint64_t end, 
            boost::system::error_code & ec)
        {
            request_.head().path = url.to_string();
            is_open(ec);
            return !ec;
        }

        void RtspSource::async_open(
            framework::string::Url const & url, 
            boost::uint64_t beg, 
            boost::uint64_t end, 
            response_type const & resp)
        {
            request_.head().path = url.to_string();
            resp_ = resp;
            framework::network::NetName addr(url.host_svc());
            if (addr.svc().empty())
                addr.svc("554");
            async_connect(addr, 
                boost::bind(&RtspSource::handle_connect, this, _1));
        }

        void RtspSource::handle_connect(
            boost::system::error_code const & ec)
        {
            LOG_DEBUG("[handle_connect] ec: " << ec.message());

            if (ec) {
                 response(ec);
                 return;
            }

            RtspSession::start();
            on_recv(RtspResponse());
        }

        void RtspSource::on_recv(
            RtspRequest const & req)
        {
            req.head().get_content(std::cout);

            RtspResponse resp;

            resp.head().get_content(std::cout);

            post(resp);
        }

        void RtspSource::on_recv(
            RtspResponse const & resp)
        {
            resp.head().get_content(std::cout);

            boost::system::error_code ec = 
                (util::protocol::rtsp_error::errors)resp.head().err_code;

            if (ec == util::protocol::rtsp_error::ok)
                ec.clear();

            if (ec) {
                 LOG_DEBUG("[on_recv] response error, ec: " << ec.message());
                 response(ec);
                 return;
            }

            RtspRequestHead & head(request_.head());
            switch (head.method) {
                case RtspRequestHead::invalid_method:
                    head.method = RtspRequestHead::describe;
                    break;
                case RtspRequestHead::describe:
                    content_base_ = resp.head()["Content-Base"];
                    if (content_base_.empty()) {
                        content_base_ = head.path + "/";
                    } else {
                        content_base_ = content_base_.substr(1, content_base_.size() - 2); // clear {}
                    }
                    parse_sdp(resp.data(), ec);
                    if (ec) break;
                    // pass down
                case RtspRequestHead::setup:
                    if (head.method == RtspRequestHead::setup) {
                        connect_transport(
                            rtp_socket_, resp.head().transport.get(), ec);
                    } else {
                        head.method = RtspRequestHead::setup;
                    }
                    if (setup_step_ < rtp_infos_.size()) {
                        head.path = content_base_ + sdp_.media(setup_step_).attr_get("control");
                        // RTP/AVP;unicast;ort=2554-2555;server_port=53016-53017;ssrc=0000456B
                        std::string transport = "RTP/AVP/TCP;unicast";
                        create_transport(
                            rtp_socket_, 
                            *this, 
                            transport, 
                            transport,
                            ec);
                        head.transport = transport;
                        ++setup_step_;
                        break;
                    }
                    head.method = RtspRequestHead::play;
                    head.path = content_base_;
                    head.transport.reset();
                    //head.range = 
                    break;
                case RtspRequestHead::play:
                    response(ec);
                    return;
                case RtspRequestHead::pause:
                    break;
                default:
                    assert(0);
            }

            if (ec) {
                response(ec);
            } else {
                head.get_content(std::cout);
                post(request_);
            }
        }

        void RtspSource::on_error(
            boost::system::error_code const & ec)
        {
            if (!resp_.empty()) {
                if (request_.head().method == RtspRequestHead::teardown) {
                    response(boost::asio::error::operation_aborted);
                } else {
                    response(ec);
                }
            }
        }

        void RtspSource::response(
            boost::system::error_code const & ec)
        {
            response_type resp;
            resp.swap(resp_);
            resp(ec);
        }

        bool RtspSource::is_open(
            boost::system::error_code & ec)
        {
            if (request_.head().method == RtspRequestHead::play) {
                ec.clear();
                return true;
            }

            return !ec;
        }

        bool RtspSource::close(
            boost::system::error_code & ec)
        {
            stop();
            return !RtspSession::close(ec);
        }

        bool RtspSource::cancel(
            boost::system::error_code & ec)
        {
            return !RtspSession::cancel_forever(ec);
        }

        size_t RtspSource::private_read_some(
            buffers_t const & buffers,
            boost::system::error_code & ec)
        {
            return rtp_socket_->read_some(
                buffers, ec);
        }

        void RtspSource::private_async_read_some(
            buffers_t const & buffers,
            handler_t const & handler)
        {
            boost::system::error_code ec;
            rtp_socket_->async_read_some( 
                buffers, handler);
        }

        bool RtspSource::set_non_block(
            bool non_block, 
            boost::system::error_code & ec)
        {
            return rtp_socket_->set_non_block(non_block, ec);
        }

        bool RtspSource::set_time_out(
            boost::uint32_t time_out, 
            boost::system::error_code & ec)
        {
            return rtp_socket_->set_time_out(time_out, ec);
        }

        bool RtspSource::continuable(
            boost::system::error_code const & ec)
        {
            return ec == boost::asio::error::would_block;
        }

        bool RtspSource::is_record() const
        {
            return false;
        }

        void RtspSource::parse_sdp(
            boost::asio::streambuf const & data, 
            boost::system::error_code & ec)
        {
            util::archive::ArchiveBuffer<> abuf(data.data());
            std::istream is(&abuf);
            if (!(is >> sdp_)) {
                ec = rtsp_error::format_error;
                return;
            }
            std::string value;
            info_.name = sdp_.desc_get('s');
            info_.type = info_.live;
            if (sdp_.attr_get("range", value)) {
            }
            info_.flags = info_.f_packet;

            size_t cnt = sdp_.media_count();
            rtp_infos_.resize(cnt);
            for (size_t i = 0; i < cnt; ++i) {
                MediaDescription md = sdp_.media(i);
                RtpInfo & rtp = rtp_infos_[i];
                int port = 0;
                md.get_rtp(rtp.type, port, rtp.format);
                md.get_rtpmap(rtp.format, rtp.codec, rtp.clock, rtp.param);
                md.get_fmtp(rtp.format, rtp.fparam);
                md.attr_get("control", rtp.control);
            }
        }

    } // namespace rtspc
} // namespace ppbox
