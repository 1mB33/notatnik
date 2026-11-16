#pragma once

#ifdef _LIB_WITH_STATE_EXPORTS
#    define _STATE_API __attribute__( ( visibility( "default" ) ) )
#else
#    define _STATE_API
#endif // !_LIB_WITH_STATE_EXPORTS

