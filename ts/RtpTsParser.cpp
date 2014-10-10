// RtpTsParser.cpp

#include "ppbox/rtspc/Common.h"
#include "ppbox/rtspc/ts/RtpTsParser.h"

#include <ppbox/demux/basic/mp2/TsDemuxer.h>
using namespace ppbox::demux;

#include "ppbox/rtspc/ts/RtpTsBuffer.h"

namespace ppbox
{
    namespace rtspc
    {

        RtpTsParser::RtpTsParser(
            boost::asio::io_service & io_svc)
            : buffer_(new RtpTsBuffer)
            , demuxer_(new TsDemuxer(io_svc, *buffer_))
        {
        }

        RtpTsParser::~RtpTsParser()
        {
        }

        void RtpTsParser::get_streams(
            std::vector<StreamInfo> & streams)
        {
            boost::system::error_code ec;
            size_t n = demuxer_->get_stream_count(ec);
            streams.resize(n);
            for (size_t i = 0; i < n; ++i) {
                demuxer_->get_stream_info(i, streams[i], ec);
            }
        }

        bool RtpTsParser::push(
            StreamInfo & info, 
            boost::system::error_code & ec)
        {
            demuxer_->open(ec);
            return true;
        }

        bool RtpTsParser::push(
            Sample & sample,
            boost::system::error_code & ec)
        {
            buffer_->put(sample);
            if (demuxer_->get_sample(sample, ec)) {
                buffer_->get(sample);
                return true;
            } else {
                return false;
            }
        }

        bool RtpTsParser::before_seek(
            Sample & sample,
            boost::system::error_code & ec)
        {
            return RtpParser::before_seek(sample, ec);
        }

    } // namespace rtspc
} // namespace ppbox
