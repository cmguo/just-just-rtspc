// RtpTsDemuxer.cpp

#include "just/rtspc/Common.h"
#include "just/rtspc/ts/RtpTsDemuxer.h"
#include "just/rtspc/ts/RtpTsParser.h"
#include "just/rtspc/RtpFilter.h"

using namespace just::demux;

#include <just/avformat/Format.h>
using namespace just::avformat;

#include <util/buffers/BuffersCopy.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.rtspc.RtpTsDemuxer", framework::logger::Debug);

namespace just
{
    namespace rtspc
    {

        RtpTsDemuxer::RtpTsDemuxer(
            boost::asio::io_service & io_svc, 
            just::data::PacketMedia & media)
            : RtpDemuxer(io_svc, media)
            , ts_parser_(NULL)
        {
        }

        RtpTsDemuxer::~RtpTsDemuxer()
        {
            if (ts_parser_) {
                delete ts_parser_;
            }
        }

        bool RtpTsDemuxer::check_open(
            boost::system::error_code & ec)
        {
            RtpDemuxer::check_open(ec);

            if (ts_parser_ == NULL) {
                ts_parser_ = new RtpTsParser(get_io_service());
                ts_parser_->open();
                add_parser(0, ts_parser_);
            }

            Sample sample;
            if (peek_sample(sample, ec)) {
                ts_parser_->get_streams(stream_infos_);
                return true;
            }

            return false;
        }

    } // namespace rtspc
} // namespace just
