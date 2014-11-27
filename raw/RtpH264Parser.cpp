// RtpH264Parser.cpp

#include "just/rtspc/Common.h"
#include "just/rtspc/raw/RtpH264Parser.h"

#include <just/avbase/stream/SampleBuffers.h>
using namespace just::avbase;
#include <just/avcodec/avc/AvcEnum.h>
using namespace just::avcodec;

#include <framework/system/BytesOrder.h>
#include <framework/string/Base64.h>
#include <framework/string/Slice.h>
using namespace framework::string;

namespace just
{
    namespace rtspc
    {

        RtpH264Parser::RtpH264Parser()
            : nalu_(AvcNaluType::is_access_end)
        {
            buf_.resize(4, 0);
            buf_used_ = 0;
            nalu_sizes_.resize(4, 0);
            nalu_count_ = 0;
        }

        RtpH264Parser::~RtpH264Parser()
        {
        }

        bool RtpH264Parser::push(
            StreamInfo & info, 
            boost::system::error_code & ec)
        {
            if (!info.format_data.empty()) {
                std::string param((char const *)&info.format_data[0], info.format_data.size());
                std::string parameter_sets;
                framework::string::map_find(param, "sprop-parameter-sets", parameter_sets, ";");
                if (!parameter_sets.empty()) {
                    std::string::size_type p = parameter_sets.find(',');
                    std::string sps = Base64::decode(parameter_sets.substr(0, p));
                    std::string pps = Base64::decode(parameter_sets.substr(p + 1));
                    std::vector<boost::uint8_t> sps_pps;
                    boost::uint8_t b0001[] = {0, 0, 0, 1};
                    sps_pps.insert(sps_pps.end(), b0001, b0001 + 4);
                    sps_pps.insert(sps_pps.end(), (boost::uint8_t *)&sps[0], (boost::uint8_t *)&sps[0] + sps.size());
                    sps_pps.insert(sps_pps.end(), b0001, b0001 + 4);
                    sps_pps.insert(sps_pps.end(), (boost::uint8_t *)&pps[0], (boost::uint8_t *)&pps[0] + pps.size());
                    config_.from_es_data(sps_pps);
                }
                info.format_data.clear();
            } else {
                info.context = &config_;
            }
            return true;
        }

        bool RtpH264Parser::push(
            Sample & sample,
            boost::system::error_code & ec)
        {
            // always set
            sample.flags |= sample.f_fragment;

            SampleBuffers::BuffersPosition pos(sample.data.begin(), sample.data.end());
            SampleBuffers::BuffersPosition end(sample.data.end());
            boost::uint8_t b1 = pos.dereference_byte();
            if ((b1 & 0x1f) == 28) { // FU-A
                pos.increment_byte(end);
                boost::uint8_t b2 = pos.dereference_byte();
                pos.increment_byte(end);
                sample.size -= 2;
                if (nalu_sizes_.size() <= nalu_count_)
                    nalu_sizes_.resize(nalu_sizes_.size() * 2);
                if (b2 & 0x80) {
                    if ((b2 & 0x1f) == AvcNaluType::IDR)
                        sample.flags |= sample.f_sync;
                    sample.size += 1;
                    if (buf_.size() <= buf_used_)
                        buf_.resize(buf_.size() * 2);
                    buf_[buf_used_] = (b1 & 0xe0) | (b2 & 0x1f);
                    RtpParser::push(boost::asio::buffer(&buf_[buf_used_], 1));
                    ++buf_used_;
                    nalu_sizes_[nalu_count_] = sample.size;
                } else if (b2 & 0x40) {
                    nalu_sizes_[nalu_count_] += sample.size;
                    ++nalu_count_;
                } else {
                    nalu_sizes_[nalu_count_] += sample.size;
                }
                RtpParser::push(SampleBuffers::range_buffers(pos, end));
            } else {
                if ((b1 & 0x1f) == AvcNaluType::IDR)
                    sample.flags |= sample.f_sync;
                RtpParser::push(sample.data);
                if (nalu_sizes_.size() <= nalu_count_)
                    nalu_sizes_.resize(nalu_sizes_.size() * 2);
                nalu_sizes_[nalu_count_] = sample.size;
                ++nalu_count_;
            }
            
            if (is_mark(sample)) {
                sample.flags |= sample.f_frag_end;
                buf_used_ = 0;
                nalu_sizes_[nalu_count_ - 1] = 0; // last nalu
                nalu_count_ = 0;
            }

            bool ret = RtpParser::push(sample, ec);
            if (sample.memory) {
                std::vector<NaluBuffer> nalus;
                SampleBuffers::BuffersPosition pos(sample.data.begin(), sample.data.end());
                SampleBuffers::BuffersPosition end(sample.data.end());
                for (size_t i = 0; nalu_sizes_[i] > 0; ++i) {
                    SampleBuffers::BuffersPosition beg = pos;
                    pos.increment_bytes(end, nalu_sizes_[i]);
                    nalus.push_back(NaluBuffer(beg, pos));
                }
                nalus.push_back(NaluBuffer(sample.size - pos.skipped_bytes(), pos, end));
                nalu_.nalus(nalus); // swap
                sample.context = &nalu_;
            }
            return ret;
        }

        bool RtpH264Parser::before_seek(
            Sample & sample,
            boost::system::error_code & ec)
        {
            buf_used_ = 0;
             nalu_count_ = 0;
            return RtpParser::before_seek(sample, ec);
        }

    } // namespace rtspc
} // namespace just
