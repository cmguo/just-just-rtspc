// RtpMpeg4GenericParser.cpp

#include "just/rtspc/Common.h"
#include "just/rtspc/raw/RtpMpeg4GenericParser.h"

#include <just/avbase/stream/SampleBuffers.h>
#include <just/avbase/stream/BitsIStream.h>
using namespace just::avbase;
#include <just/avcodec/avc/AvcEnum.h>
using namespace just::avcodec;

#include <util/buffers/CycleBuffers.h>

#include <framework/system/BytesOrder.h>
#include <framework/string/Base16.h>
#include <framework/string/Slice.h>
using namespace framework::string;

#include <algorithm>

namespace just
{
    namespace rtspc
    {

        RtpMpeg4GenericParser::RtpMpeg4GenericParser()
            : size_length_(0)
            , index_length_(0)
            , index_delta_length_(0)
        {
        }

        RtpMpeg4GenericParser::~RtpMpeg4GenericParser()
        {
        }

        bool RtpMpeg4GenericParser::push(
            StreamInfo & info, 
            boost::system::error_code & ec)
        {
            if (size_length_ == 0) {
                std::string param((char const *)&info.format_data[0], info.format_data.size());
                transform(param.begin(), param.end(), param.begin(), tolower);
                std::string config;
                framework::string::map_find(param, "config", config, ";");
                framework::string::map_find(param, "sizelength", size_length_, ";");
                framework::string::map_find(param, "indexlength", index_length_, ";");
                framework::string::map_find(param, "indexdeltalength", index_delta_length_, ";");
                config = Base16::decode(config);
                info.format_data.clear();
                info.format_data.insert(info.format_data.end(), 
                    (boost::uint8_t *)&config[0], (boost::uint8_t *)&config[0] + config.size());
            }
            return true;
        }

        struct Const
        {
            Const(size_t n) : n(n) {}
            size_t operator()() { return n; }
            size_t n;
        };

        bool RtpMpeg4GenericParser::push(
            Sample & sample,
            boost::system::error_code & ec)
        {
            util::buffers::CycleBuffers<std::deque<boost::asio::const_buffer>, boost::uint8_t> buf(sample.data);
            buf.commit(sample.size);
            BitsIStream<boost::uint8_t> bits_is(buf);

            SampleBuffers::BuffersPosition pos(sample.data.begin(), sample.data.end());
            SampleBuffers::BuffersPosition end(sample.data.end());

            U<16> headers_length;
            UV<Const> size(size_length_);
            UV<Const> index(index_length_);
            UV<Const> index_delta(index_delta_length_);
            while (!pos.at_end()) {
                bits_is >> headers_length
                    >> size 
                    >> index;
                size_t header_size = 2 + headers_length / 8;
                sample.size -= header_size;
                pos.increment_bytes(end, header_size);;
                SampleBuffers::BuffersPosition pos2(pos);
                pos.increment_bytes(end, size);
                bits_is.seekg(pos.skipped_bytes(), std::ios::beg);
                RtpParser::push(SampleBuffers::range_buffers(pos2, pos));
            }

            if (is_mark(sample)) {
                sample.flags |= sample.f_frag_end;
            }

            bool ret = RtpParser::push(sample, ec);
            if (sample.memory) {
            }
            return ret;
        }

        bool RtpMpeg4GenericParser::before_seek(
            Sample & sample,
            boost::system::error_code & ec)
        {
            return RtpParser::before_seek(sample, ec);
        }

    } // namespace rtspc
} // namespace just
