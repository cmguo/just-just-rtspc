// RtspcModule.cpp

#include "just/rtspc/Common.h"
#include "just/rtspc/RtspcModule.h"
#include "just/rtspc/ClassRegister.h"

namespace just
{
    namespace rtspc
    {

        RtspcModule::RtspcModule(
            util::daemon::Daemon & daemon)
            : just::common::CommonModuleBase<RtspcModule>(daemon, "RtspcModule")
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
} // namespace just
