// RtspcModule.h

#ifndef _PPBOX_RTSPC_RTSPC_MODULE_H_
#define _PPBOX_RTSPC_RTSPC_MODULE_H_

namespace ppbox
{
    namespace rtspc
    {

        class RtspcModule 
            : public ppbox::common::CommonModuleBase<RtspcModule>
        {
        public:
            RtspcModule(
                util::daemon::Daemon & daemon);

            virtual ~RtspcModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();
        };

    } // namespace rtspc
} // namespace ppbox

#endif // _PPBOX_RTSPC_RTSPC_MODULE_H_