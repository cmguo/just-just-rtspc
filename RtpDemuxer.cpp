// RtpDemuxer.cpp

#include "ppbox/rtspc/Common.h"
#include "ppbox/rtspc/RtpDemuxer.h"
#include "ppbox/rtspc/RtpFilter.h"
#include "ppbox/rtspc/RtpParser.h"
#include "ppbox/rtspc/RtspMedia.h"

#include <util/tools/ClassRegister.h>

#include "ppbox/rtspc/RtpH264Parser.h"

using namespace ppbox::demux;

#include <ppbox/avformat/Format.h>
using namespace ppbox::avformat;

#include <util/buffers/BuffersCopy.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.rtspc.RtpDemuxer", framework::logger::Debug);

namespace ppbox
{
    namespace rtspc
    {

        RtpDemuxer::RtpDemuxer(
            boost::asio::io_service & io_svc, 
            ppbox::data::PacketMedia & media)
            : PacketDemuxer(io_svc, media)
            , filter_(NULL)
        {
        }

        RtpDemuxer::~RtpDemuxer()
        {
            if (filter_) {
                delete filter_;
            }
        }

        bool RtpDemuxer::check_open(
            boost::system::error_code & ec)
        {
            RtspMedia const & media(static_cast<RtspMedia const &>(PacketDemuxer::media()));

            std::vector<RtpInfo> const & rtp_infos = 
                media.rtsp_source().rtp_infos();

            using namespace ppbox::avbase;
            using namespace framework::string;

            if (filter_ == NULL) {
                filter_ = new RtpFilter;
                add_filter(filter_);
            }

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
                        filter_->add_stream(stream, parser);
                }
            }

            return true;
        }

    } // namespace rtspc
} // namespace ppbox
