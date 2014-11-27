// RtpParser.h

#ifndef _JUST_RTSPC_RTP_PARSER_H_
#define _JUST_RTSPC_RTP_PARSER_H_

#include <just/demux/base/DemuxBase.h>

#include <util/tools/ClassFactory.h>

namespace just
{
    namespace rtspc
    {

        class RtpParser
        {
        public:
            RtpParser();

            virtual ~RtpParser();

        public:
            virtual bool push(
                just::demux::StreamInfo & info,
                boost::system::error_code & ec);

            virtual bool push(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool before_seek(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

        protected:
            static bool is_mark(
                just::demux::Sample & sample);

            template <typename ConstBuffers>
            void push(
                ConstBuffers const & buffers)
            {
                sample_.data.insert(sample_.data.end(), buffers.begin(), buffers.end());
            }

        private:
            just::demux::Sample sample_;
        };

        struct RtpParserTraits
            : util::tools::ClassFactoryTraits
        {
            typedef std::string key_type;
            typedef RtpParser * (create_proto)();

            static boost::system::error_code error_not_found();
        };

        typedef util::tools::ClassFactory<RtpParserTraits> RtpParserFactory;

    } // namespace rtspc
} // namespace just

#define JUST_REGISTER_RTP_PARSER(key, cls) UTIL_REGISTER_CLASS(just::rtspc::RtpParserFactory, key, cls)

#endif // _JUST_RTSPC_RTP_PARSER_H_
