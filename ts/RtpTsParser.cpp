// RtpTsParser.cpp

#include "just/rtspc/Common.h"
#include "just/rtspc/ts/RtpTsParser.h"

#include <just/demux/basic/mp2/TsDemuxer.h>
using namespace just::demux;
#include <just/avformat/Error.h>

#include "just/rtspc/ts/RtpTsBuffer.h"

#include <fstream>

namespace just
{
    namespace rtspc
    {

        std::ofstream ofs;

        RtpTsParser::RtpTsParser(
            boost::asio::io_service & io_svc)
            : buffer_(new RtpTsBuffer)
            , demuxer_(new TsDemuxer(io_svc, *buffer_))
        {
        }

        RtpTsParser::~RtpTsParser()
        {
        }

        void RtpTsParser::open()
        {
            ofs.open("wfd.ts", std::ios::binary);
            boost::system::error_code ec;
            demuxer_->open(ec);
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
            ec.clear();
            return true;
        }

        bool RtpTsParser::push(
            Sample & sample,
            boost::system::error_code & ec)
        {
            for (size_t i = 0; i < sample.data.size(); ++i) {
                boost::asio::const_buffer const & buf(sample.data[i]);
                ofs.write(boost::asio::buffer_cast<char const *>(buf), boost::asio::buffer_size(buf));
            }
            buffer_->put(sample);
            if (!demuxer_->get_sample(sample, ec)) {
                buffer_->get(sample);
                return true;
            } else if (ec == just::avformat::error::file_stream_error) {
                ec.clear();
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
} // namespace just
