// RtspcModule.h

#ifndef _JUST_RTSPC_RTSPC_MODULE_H_
#define _JUST_RTSPC_RTSPC_MODULE_H_

namespace just
{
    namespace rtspc
    {

        class RtspcModule 
            : public just::common::CommonModuleBase<RtspcModule>
        {
        public:
            RtspcModule(
                util::daemon::Daemon & daemon);

            virtual ~RtspcModule();

        public:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);
        };

    } // namespace rtspc
} // namespace just

#endif // _JUST_RTSPC_RTSPC_MODULE_H_
