// RtpTsDemuxer.cpp

#include "ppbox/rtspc/Common.h"
#include "ppbox/rtspc/ts/RtpTsDemuxer.h"
#include "ppbox/rtspc/ts/RtpTsParser.h"
#include "ppbox/rtspc/RtpFilter.h"

using namespace ppbox::demux;

#include <ppbox/avformat/Format.h>
using namespace ppbox::avformat;

#include <util/buffers/BuffersCopy.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.rtspc.RtpTsDemuxer", framework::logger::Debug);

namespace ppbox
{
    namespace rtspc
    {

        RtpTsDemuxer::RtpTsDemuxer(
            boost::asio::io_service & io_svc, 
            ppbox::data::PacketMedia & media)
            : RtpDemuxer(io_svc, media)
            , ts_parser_(new RtpTsParser(io_svc))
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

            add_parser(0, ts_parser_);

            Sample sample;
            if (peek_sample(sample, ec)) {
                ts_parser_->get_streams(stream_infos_);
                return true;
            }

            return false;
        }

    } // namespace rtspc
} // namespace ppbox
