// RtspcModule.cpp

#include "ppbox/rtspc/Common.h"
#include "ppbox/rtspc/RtspcModule.h"
#include "ppbox/rtspc/ClassRegister.h"

namespace ppbox
{
    namespace rtspc
    {

        RtspcModule::RtspcModule(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<RtspcModule>(daemon, "RtspcModule")
        {
        }

        RtspcModule::~RtspcModule()
        {
        }

        boost::system::error_code RtspcModule::startup()
        {
            boost::system::error_code ec;
            return ec;
        }

        void RtspcModule::shutdown()
        {
        }

    } // namespace rtspc
} // namespace ppbox
