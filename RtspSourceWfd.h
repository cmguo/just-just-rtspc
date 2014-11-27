// RtspSourceWfd.h

#ifndef _JUST_RTSPC_RTSP_SOURCE_WFD_H_
#define _JUST_RTSPC_RTSP_SOURCE_WFD_H_

#include "just/rtspc/RtspSource.h" 

namespace just
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

        JUST_REGISTER_RTSP_SOURCE("wfd", RtspSourceWfd);

    } // namespace data
} // namespace just

#endif // _JUST_RTSPC_RTSP_SOURCE_WFD_H_
