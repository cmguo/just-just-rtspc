// RtpInfo.h

#ifndef _PPBOX_RTSPC_RTP_INFO_H_
#define _PPBOX_RTSPC_RTP_INFO_H_

namespace ppbox
{
    namespace rtspc
    {

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

            RtpInfo()
                : format(0)
                , clock(0)
                , ssrc(0)
                , sequence(0)
                , timestamp(0)
            {
            }
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_RTSPC_RTP_INFO_H_
