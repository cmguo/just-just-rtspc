// RtpParser.cpp

#include "ppbox/rtspc/Common.h"
#include "ppbox/rtspc/RtpParser.h"
#include "ppbox/rtspc/RtpParser.h"

#include <ppbox/avformat/Error.h>
using namespace ppbox::avformat;
using namespace ppbox::demux;

#include <util/protocol/rtsp/rtp/RtpPacket.h>

#include <framework/system/BytesOrder.h>

namespace ppbox
{
    namespace rtspc
    {

        RtpParser::RtpParser()
        {
        }

        RtpParser::~RtpParser()
        {
        }

        bool RtpParser::push(
            StreamInfo & info, 
            boost::system::error_code & ec)
        {
            return true;
        }

        bool RtpParser::push(
            Sample & sample,
            boost::system::error_code & ec)
        {
            sample_.size += sample.size;
            sample_.append(sample);
            sample_.flags |= (sample.flags & 0xf);

            if (sample.flags & sample.f_multiple) {
            } else if (sample.flags & sample.f_fragment) {
                sample_.flags |= sample.f_fragment;
                if (sample.flags & sample.f_discontinuity)
                    sample_.flags |= sample.f_discontinuity | sample.f_frag_lost;
                if (sample.flags & sample.f_frag_end) {
                    if ((sample_.flags & sample.f_frag_lost) == 0) {
                        sample.flags = (sample_.flags & 0xf);
                        sample.size = sample_.size;
                        sample.data.swap(sample_.data);
                        sample.append(sample_);
                    }
                    sample_.size = 0;
                    sample_.data.clear();
                    // only keep f_discontinuity
                    sample_.flags &= sample.f_discontinuity;
                }
            } else {
                sample.flags = (sample_.flags & 0xf);
                sample.size = sample_.size;
                sample.data.swap(sample_.data);
                sample.append(sample_);
                sample_.size = 0;
                sample_.data.clear();
                // only keep f_discontinuity
                sample_.flags &= sample.f_discontinuity;
            }

            if (sample.memory) {
                if (sample_.flags & sample.f_discontinuity) {
                    sample.flags |= sample.f_discontinuity;
                    sample_.flags &= ~sample.f_discontinuity;
                }
            }

            ec.clear();
            return true;
        }

        bool RtpParser::before_seek(
            Sample & sample,
            boost::system::error_code & ec)
        {
            sample.append(sample_);
            sample.data.clear();
            sample.flags = 0;
            ec.clear();
            return true;
        }

        bool RtpParser::is_mark(
            Sample & sample)
        {
            using util::protocol::RtpHead;
            RtpHead const * head = (RtpHead const *)&sample.context;
            return (head->mpt & 0x80) != 0;
        }

        boost::system::error_code RtpParserTraits::error_not_found()
        {
            return error::codec_not_support;
        }

    } // namespace rtspc
} // namespace ppbox
