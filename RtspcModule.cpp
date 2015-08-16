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

        bool RtspcModule::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool RtspcModule::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

    } // namespace rtspc
} // namespace just
