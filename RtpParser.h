// RtpParser.h

#ifndef _PPBOX_RTSPC_RTP_PARSER_H_
#define _PPBOX_RTSPC_RTP_PARSER_H_

#include <ppbox/demux/base/DemuxBase.h>

#include <util/tools/ClassFactory.h>

namespace ppbox
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
                ppbox::demux::StreamInfo & info,
                boost::system::error_code & ec);

            virtual bool push(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool before_seek(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

        protected:
            static bool is_mark(
                ppbox::demux::Sample & sample);

            template <typename ConstBuffers>
            void push(
                ConstBuffers const & buffers)
            {
                sample_.data.insert(sample_.data.end(), buffers.begin(), buffers.end());
            }

        private:
            ppbox::demux::Sample sample_;
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
} // namespace ppbox

#define PPBOX_REGISTER_RTP_PARSER(key, cls) UTIL_REGISTER_CLASS(ppbox::rtspc::RtpParserFactory, key, cls)

#endif // _PPBOX_RTSPC_RTP_PARSER_H_
