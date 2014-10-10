// RtpTsDemuxer.h

#ifndef _PPBOX_RTSPC_TS_RTP_TS_DEMUXER_H_
#define _PPBOX_RTSPC_TS_RTP_TS_DEMUXER_H_

#include "util/buffers/StlBuffer.h"

namespace ppbox
{
    namespace rtspc
    {

        class RtpTsBuffer
            : public util::buffers::StlStream<boost::uint8_t>
        {
        public:
            RtpTsBuffer()
                : iter_(packets_)
                , buf_(*this)
            {
            }

        public:
            void put(
                Sample & sample)
            {
                packets_.push_back(sample);
                iter_.end += sample.size;
            }

            void get(
                Sample & sample)
            {
                std::vector<ppbox::data::DataBlock> & blocks = 
                    *(std::vector<ppbox::data::DataBlock> *)sample.context;

                std::deque<boost::asio::const_buffer> datas;
                BufferIterator iter(iter_);
                boost::asio::const_buffer const & buf(iter.buf);
                for (size_t i = 0; i < blocks.size(); ++i) {
                    pos_type pos = blocks[i].offset;
                    assert(!iter.invalid(pos));
                    iter.set(pos);
                    size_t size = blocks[i].size;
                    while (size > boost::asio::buffer_size(buf)) {
                        datas.push_back(buf);
                        size -= boost::asio::buffer_size(buf);
                        iter.inc();
                    }
                    datas.push_back(boost::asio::buffer(buf, size));
                }
                sample.data.swap(datas);

                boost::uint64_t pos = blocks.front().offset;
                if (offsets_.size() <= sample.itrack)
                    offsets_.resize(sample.itrack + 1, boost::uint64_t(-1));
                offsets_[sample.itrack] = pos;
                for (size_t i = 0; i < offsets_.size(); ++i) {
                    if (offsets_[i] < pos)
                        pos = offsets_[i];
                }
                assert(!iter_.invalid(pos));
                off_type off = pos_type(pos) - iter_.pos;
                while (off >= packets_.front().size) {
                    off -= packets_.front().size;
                    sample.append(packets_.front());
                    iter_.drop();
                    packets_.pop_front();
                }
            }

        protected:
            virtual int_type underflow()
            {
                if (iter_.pos2 + (off_type)buf_.offset() < iter_.end) {
                    iter_.inc();
                    buf_ = iter_.buf;
                    return traits_type::to_int_type(*this->gptr());
                }
                return traits_type::eof();
            }

            virtual pos_type seekoff(
                off_type off, 
                std::ios_base::seekdir dir,
                std::ios_base::openmode mode)
            {
                if (dir == std::ios_base::cur) {
                    pos_type pos = iter_.pos2 + (off_type)buf_.offset();
                    if (off == 0) {
                        return pos;
                    }
                    pos += off;
                    return seekpos(pos, mode);
                } else if (dir == std::ios_base::beg) {
                    return seekpos(off, mode);
                } else if (dir == std::ios_base::end) {
                    assert(off <= 0);
                    pos_type pos = iter_.end;
                    return seekpos(pos + off, mode);
                } else {
                    return pos_type(-1);
                }
            }

            virtual pos_type seekpos(
                pos_type position, 
                std::ios_base::openmode mode)
            {
                assert(position != pos_type(-1));
                if (iter_.invalid(position)) {
                    return pos_type(-1);
                }
                if (mode == std::ios_base::in) {
                    iter_.set(position);
                    buf_ = iter_.buf;
                    return position;
                } else if (mode == std::ios_base::out) {
                    return pos_type(-1);
                } else {
                    return pos_type(-1);
                }
            }

        private:
            struct BufferIterator
            {
                std::deque<Sample> const & packets_;
                pos_type pos;
                pos_type end;
                size_t index1;
                pos_type pos1;
                size_t index2;
                pos_type pos2;
                boost::asio::const_buffer buf;

                BufferIterator(
                    std::deque<Sample> packets)
                    : packets_(packets)
                    , pos(0)
                    , end(0)
                    , index1(0)
                    , pos1(0)
                    , index2(0)
                    , pos2(0)
                {
                }

                bool invalid(
                    pos_type p) const
                {
                    return p < pos || p > end;
                }

                void set(
                    pos_type p)
                {
                    if (p < pos1) {
                        index1 = 0;
                        index2 = 0;
                        pos1 = pos;
                        pos2 = pos;
                    }
                    boost::uint64_t off = p - pos1;
                    while (off > packets_[index1].size) {
                        off -= packets_[index1].size;
                        pos1 += packets_[index1].size;
                        ++index1;
                        index2 = 0;
                        pos2 = pos1;
                    }
                    std::deque<boost::asio::const_buffer> const & packet = packets_[index1].data;
                    off = p - pos2;
                    while (off > boost::asio::buffer_size(packet[index2])) {
                        off -= boost::asio::buffer_size(packet[index2]);
                        pos2 += boost::asio::buffer_size(packet[index2]);
                        ++index2;
                    }
                    buf = packet[index2] + off;
                }

                void inc()
                {
                    do {
                        pos2 += boost::asio::buffer_size(buf);
                        if (++index2 >= packets_[index1].data.size()) {
                            pos1 += packets_[index1].size;
                            ++index1;
                            index2 = 0;
                        }
                        buf = packets_[index1].data[index2];
                    } while (boost::asio::buffer_size(buf) == 0);
                }

                void drop()
                {
                    pos += packets_.front().size;
                    --index1;
                }
            };

        private:
            std::deque<Sample> packets_;
            BufferIterator iter_;
            util::buffers::StlBuffer<
                util::buffers::detail::_write, 
                boost::uint8_t> buf_;
            std::vector<boost::uint64_t> offsets_;
        };

    } // namespace rtspc
} // namespace ppbox

#endif // _PPBOX_RTSPC_TS_RTP_TS_DEMUXER_H_
