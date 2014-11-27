// RtspSourceWfd.cpp

#include "just/rtspc/Common.h"
#include "just/rtspc/RtspSourceWfd.h"

#include <util/protocol/rtsp/RtspTransport.h>
using namespace util::protocol;
#include <util/archive/ArchiveBuffer.h>

#include <framework/network/NetName.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/StringRecord.h>

namespace just
{
    namespace rtspc
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.rtspc.RtspSourceWfd", framework::logger::Debug);

        using util::protocol::rtsp_field::f_range;
        using util::protocol::rtsp_field::f_transport;

        RtspSourceWfd::RtspSourceWfd(
            boost::asio::io_service & io_svc)
            : RtspSource(io_svc)
        {
        }

        RtspSourceWfd::~RtspSourceWfd()
        {
        }

        void RtspSourceWfd::on_connect()
        {
            // wait request
        }

        void RtspSourceWfd::on_recv(
            RtspRequest const & req)
        {
            req.head().get_content(std::cout);

            RtspResponse resp;

            RtspRequestHead const & req_head(req.head());
            RtspResponseHead & resp_head(resp.head());
            RtspRequestHead & req2_head(request_.head());
            switch (req_head.method) {
                case RtspRequestHead::options:
                    resp_head["Public"] = "org.wfa.wfd1.0, GET_PARAMETER, SET_PARAMETER";
                    // request
                    req2_head.method = RtspRequestHead::options;
                    req2_head["Require"] = "org.wfa.wfd1.0";
                    break;
                case RtspRequestHead::get_parameter:
                    resp_head["Content-Type"] = "text/parameters";
                    {
#define CEA_HIGH_RESOLUTION     "0001DEFF"
#define CEA_NORMAL_RESOLUTION   "00008C7F"
#define VESA_HIGH_RESOLUTION    "157C7FFF"
#define VESA_NORMAL_RESOLUTION  "00007FFF"
#define HH_HIGH_RESOLUTION      "00000FFF"
#define HH_NORMAL_RESOLUTION    "00000FFF"
                        std::ostream os(&resp.data());
                        char const * CEA = CEA_NORMAL_RESOLUTION;
                        char const * VESA = VESA_NORMAL_RESOLUTION;
                        char const * HH = HH_NORMAL_RESOLUTION;
                        os << "wfd_video_formats:"
                            << " 28 00" // native prefer_display_mode
                            << " 02 02" // h264_profile h264_level
                            << " " << CEA
                            << " " << VESA
                            << " " << HH
                            << " 00 0000 0000 11 none none" // latency min_slice slice_enc frame_control
                            << "," // another one
                            << " 01  02"
                            << " " << CEA
                            << " " << VESA
                            << " " << HH
                            << " 00 0000 0000 11 none none"
                            << "\r\n";
                        os << "wfd_audio_codecs:"
                            << " LPCM 00000003 00\r\n";
                        std::string transport = "RTP/AVP/UDP;unicast";
                        boost::system::error_code ec;
                        create_transport(
                            rtp_socket_, 
                            *this, 
                            transport, 
                            transport,
                            ec);
                        req2_head[f_transport] = transport; // save for future
                        transport.replace(transport.find(";client_port="), 13, " ");
                        transport.erase(transport.find('-'));
                        os << "wfd_client_rtp_ports:" 
                            << " " <<  transport << " 0 mode=play\r\n";
                        os << "wfd_uibc_capability:" 
                            << " input_category_list=GENERIC"
                            << ";generic_cap_list=Keyboard, Mouse, SingleTouch"
                            << ";hidc_cap_list=none;port=none\r\n";
                        //os << "wfd_standby_resume_capability: supported\r\n";
                    }
                    break;
                case RtspRequestHead::set_parameter:
                    {
                        util::archive::ArchiveBuffer<> abuf(req.data().data());
                        std::istream is(&abuf);
                        std::string param;
                        while (std::getline(is, param)) {
                            if (false) {
                            } else if (param.compare(0, 21, "wfd_presentation_URL:") == 0) {
                                content_base_ = param.substr(22, param.find(' ', 22) - 22);
                                req2_head.path = content_base_;
                            } else if (param.compare(0, 19, "wfd_trigger_method:") == 0) {
                                req2_head.method = RtspRequestHead::setup;
                                ++setup_step_;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }

            post(resp);

            if (req2_head.method != RtspRequestHead::invalid_method) {
                post(request_);
            }
        }

        void RtspSourceWfd::on_recv(
            RtspResponse const & resp)
        {
            RtspRequestHead & head(request_.head());
            switch (head.method) {
                case RtspRequestHead::options:
                    head.erase("Require");
                    head.method = head.invalid_method;
                default:
                    break;
            }

            RtspSource::on_recv(resp);
        }

    } // namespace rtspc
} // namespace just
