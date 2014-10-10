// RtspSourceWfd.h

#ifndef _PPBOX_RTSPC_RTSP_SOURCE_WFD_H_
#define _PPBOX_RTSPC_RTSP_SOURCE_WFD_H_

#include "ppbox/rtspc/RtspSource.h" 

namespace ppbox
{
    namespace rtspc
    {

        class RtspSourceWfd
            : public RtspSource
        {
        public:
            RtspSourceWfd(
                boost::asio::io_service & io_svc);

            virtual ~RtspSourceWfd();

        private:
            // RtspSource
            virtual void on_connect();

            // RtspSession
            virtual void on_recv(
                RtspRequest const & req);

            virtual void on_recv(
                RtspResponse const & resp);
        };

        PPBOX_REGISTER_RTSP_SOURCE("wfd", RtspSourceWfd);

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_RTSPC_RTSP_SOURCE_WFD_H_
