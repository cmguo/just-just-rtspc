// RtpFilter.cpp

#include "ppbox/rtspc/Common.h"
#include "ppbox/rtspc/RtpFilter.h"
#include "ppbox/rtspc/RtpParser.h"

using namespace ppbox::demux;

#include <framework/system/BytesOrder.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/DataRecord.h>

namespace ppbox
{
    namespace rtspc
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.rtspc.RtpFilter", framework::logger::Debug);

        RtpFilter::RtpFilter()
            : ready_session_(NULL)
        {
        }

        RtpFilter::~RtpFilter()
        {
        }

        void RtpFilter::add_stream(
            StreamInfo & info, 
            RtpParser * parser)
        {
            if (info.index >= sessions_.size()) {
                sessions_.resize(info.index + 1);
            }
            sessions_[info.index].parser = parser;
        }

        bool RtpFilter::get_sample(
            Sample & sample,
            boost::system::error_code & ec)
        {
            while (get_sample2(sample, ec)) {
                RtpSession & session = sessions_[sample.itrack];
                if (session.parser == NULL)
                    continue;
                if (session.parser->push(sample, ec)) {
                    if (sample.memory) {
                        sample.append(rtcp_sample_);
                        LOG_TRACE("[get_sample] time: " << sample.dts << " size: " << sample.size);
                        return true;
                    }
                } else {
                    return false;
                }
            }
            return false;
        }

        bool RtpFilter::get_sample2(
            Sample & sample,
            boost::system::error_code & ec)
        {
            if (ready_session_) {
                LOG_DEBUG("[get_sample2] ready packet " << ready_session_->seq_base);
                sample = ready_session_->samples.front();
                ++ready_session_->seq_base;
                ready_session_->samples.pop_front();
                if (!ready_session_->ready()) {
                    ready_session_ = NULL;
                }
                ec.clear();
                return true;
            }

            while (Filter::get_sample(sample, ec)) {
                if (!parse(sample, ec))
                    continue;
                RtpSession & session = sessions_[sample.itrack];
                //LOG_TRACE("[get_sample2] sequence: " << rtp_head_.sequence);
                if (session.seq_base == rtp_head_.sequence) {
                    ++session.seq_base;
                    if (!session.samples.empty()) {
                        session.samples.pop_front();
                    }
                } else if (rtp_head_.sequence < session.seq_base) {
                    LOG_DEBUG("[get_sample2] late packet " << rtp_head_.sequence << " to " << session.seq_base);
                } else {
                    LOG_DEBUG("[get_sample2] unordered packet " << rtp_head_.sequence << " to " << session.seq_base);
                    for (; 8 < rtp_head_.sequence - session.seq_base; ++session.seq_base) {
                        if (session.ready()) {
                            break;
                        }
                        LOG_DEBUG("[get_sample2] missed packet " << session.seq_base);
                        if (!session.samples.empty()) {
                            session.samples.pop_front();
                        }
                    }
                    if (session.samples.size() <= (size_t)(rtp_head_.sequence - session.seq_base))
                        session.samples.resize(rtp_head_.sequence - session.seq_base + 1);
                    session.samples[rtp_head_.sequence - session.seq_base] = sample;
                    if (session.ready()) {
                        LOG_DEBUG("[get_sample2] discontinuity packet " << session.seq_base);
                        sample = session.samples.front();
                        sample.flags |= sample.f_discontinuity;
                        ++session.seq_base;
                        session.samples.pop_front();
                    }
                }

                if (sample.memory) {
                    if (session.ready()) {
                        ready_session_ = &session;
                    }
                    break;
                }
            } // while

            return sample.memory != NULL;
        }

        bool RtpFilter::get_next_sample(
            Sample & sample,
            boost::system::error_code & ec)
        {
            if (!Filter::get_next_sample(sample, ec))
                return false;
            parse(sample, ec);
            return true;
        }

        bool RtpFilter::get_last_sample(
            Sample & sample,
            boost::system::error_code & ec)
        {
            if (!Filter::get_last_sample(sample, ec))
                return false;
            parse(sample, ec);
            return true;
        }

        bool RtpFilter::before_seek(
            Sample & sample,
            boost::system::error_code & ec)
        {
            for (size_t i = 0; i < sessions_.size(); ++i) {
                RtpSession & session = sessions_[i];
                if (session.parser)
                    session.parser->before_seek(sample, ec);
                for (size_t j = 0; j < session.samples.size(); ++j) {
                    sample.append(session.samples[j]);
                }
                session.samples.clear();
            }
            return Filter::before_seek(sample, ec);
        }

        bool RtpFilter::parse(
            Sample & sample,
            boost::system::error_code & ec)
        {
            boost::uint8_t const * header = 
                boost::asio::buffer_cast<boost::uint8_t const *>(sample.data.front());
            if ((header[1] & 1) == 0) {
                rtp_head_ = *(util::protocol::RtpHead const *)(header + 4);
                rtp_head_.sequence = framework::system::BytesOrder::big_endian_to_host(rtp_head_.sequence);
                rtp_head_.timestamp = framework::system::BytesOrder::big_endian_to_host(rtp_head_.timestamp);
                rtp_head_.ssrc = framework::system::BytesOrder::big_endian_to_host(rtp_head_.ssrc);
            } else {
                //ppbox::data::PacketSource * packet_source = (ppbox::data::PacketSource *)sample.context;
                //RtspSource & source = static_cast<RtspSource &>(packet_source->source());
                //util::stream::Dual & rtp_socket = source.rtp_socket();
                //rtp_socket.write_some(rtcp);
                rtcp_sample_.append(sample);
                return false;
            }

            sample.flags = 0;
            sample.size -= 4 + sizeof(rtp_head_);
            sample.itrack = header[1] >> 1;
            sample.dts = rtp_head_.timestamp;
            sample.duration = 0;
            sample.data.front() = sample.data.front() + 4 + sizeof(rtp_head_);
            sample.context = *(void **)&rtp_head_;

            LOG_TRACE("[parse] itrack: " << sample.itrack 
                << " sequence: " << rtp_head_.sequence
                << " size: " << sample.size 
                << " mark: " << int(rtp_head_.mpt >> 7));
            return true;
        }

    } // namespace rtspc
} // namespace ppbox
