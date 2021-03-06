// RtpRawDemuxer.cpp

#include "just/rtspc/Common.h"
#include "just/rtspc/raw/RtpRawDemuxer.h"
#include "just/rtspc/RtpFilter.h"
#include "just/rtspc/RtspMedia.h"

#include <util/tools/ClassRegister.h>

#include "just/rtspc/raw/RtpH264Parser.h"
#include "just/rtspc/raw/RtpMpeg4GenericParser.h"

using namespace just::demux;

#include <just/avformat/Format.h>
using namespace just::avformat;

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.rtspc.RtpRawDemuxer", framework::logger::Debug);

namespace just
{
    namespace rtspc
    {

        RtpRawDemuxer::RtpRawDemuxer(
            boost::asio::io_service & io_svc, 
            just::data::PacketMedia & media)
            : RtpDemuxer(io_svc, media)
        {
        }

        RtpRawDemuxer::~RtpRawDemuxer()
        {
        }

        bool RtpRawDemuxer::check_open(
            boost::system::error_code & ec)
        {
            RtpDemuxer::check_open(ec);

            RtspMedia const & media(static_cast<RtspMedia const &>(PacketDemuxer::media()));

            std::vector<RtpInfo> const & rtp_infos = 
                media.rtsp_source().rtp_infos();

            using namespace just::avbase;
            using namespace framework::string;

            for (size_t i = 0; i < rtp_infos.size(); ++i) {
                RtpInfo const & rtp = rtp_infos[i];
                StreamInfo stream;
                stream.index = boost::uint32_t(i);
                if (rtp.type == "video") {
                    stream.type = StreamType::VIDE;
                } else if (rtp.type == "audio") {
                    stream.type = StreamType::AUDI;
                    if (!rtp.param.empty()) {
                        stream.audio_format.channel_count = parse<boost::uint32_t>(rtp.param);
                    }
                } else {
                    continue;
                }
                stream.time_scale = rtp.clock;
                stream.format_data.resize(rtp.fparam.size());
                memcpy(&stream.format_data[0], rtp.fparam.c_str(), rtp.fparam.size());
                stream.context = rtp.codec.c_str();
                RtpParser * parser = RtpParserFactory::create(rtp_infos[i].codec, ec);
                if (parser && parser->push(stream, ec)
                    && Format::finish_from_stream(stream, "rtp", 0, ec)
                    && parser->push(stream, ec)) {
                        stream_infos_.push_back(stream);
                        add_parser(i, parser);
                }
            }

            return true;
        }

    } // namespace rtspc
} // namespace just
