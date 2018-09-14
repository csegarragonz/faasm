#include "wasm.h"

#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/syscall.h>

#include <atomic>

#include "Runtime/RuntimeData.h"
#include "Runtime/Intrinsics.h"

using namespace IR;
using namespace Runtime;


namespace wasm {
    DEFINE_INTRINSIC_MODULE(env)

    // ------------------------
    // I/O
    // ------------------------

    DEFINE_INTRINSIC_FUNCTION_WITH_MEM_AND_TABLE(env, "__syscall_writev", I32, __syscall_writev,
            I32 a, I32 b, I32 c) {
        printf("SYSCALL - writev %i %i %i\n", a, b, c);
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__syscall_ioctl", I32, __syscall_ioctl,
                                                 I32 a, I32 b, I32 c, I32 d, I32 e, I32 f) {
        printf("SYSCALL - ioctl %i %i %i %i %i %i\n", a, b, c, d, e, f);
        return 0;
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__syscall_poll", I32, __syscall_poll, I32 a, I32 b, I32 c) {
        printf("SYSCALL - poll %i %i %i\n", a, b, c);
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__syscall_open", I32, __syscall_open, I32 a, I32 b, I32 c) {
        printf("SYSCALL - open %i %i %i\n", a, b, c);
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__syscall_llseek", I32, __syscall_llseek,
                              I32 a, I32 b, I32 c, I32 d, I32 e) {
        printf("SYSCALL - llseek %i %i %i %i %i\n", a, b, c, d, e);
        return 0;
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__syscall_close", I32, __syscall_close, I32 a) {
        printf("SYSCALL - close %i\n", a);
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__syscall_futex", I32, __syscall_futex,
                              I32 a, I32 b, I32 c, I32 d, I32 e, I32 f) {
        printf("SYSCALL - futex %i %i %i %i %i %i\n", a, b, c, d, e, f);
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    // ------------------------
    // Sockets/ network
    // ------------------------

    DEFINE_INTRINSIC_FUNCTION_WITH_MEM_AND_TABLE(env, "__syscall_socketcall", I32, __syscall_socketcall, I32 syscallNo,
                                                 I32 argsPtr) {
        MemoryInstance *memory = Runtime::getMemoryFromRuntimeData(contextRuntimeData, defaultMemoryId.id);
        U32 *args = memoryArrayPtr<U32>(memory, argsPtr, 2);
        U32 call = args[0];
        U32 callArgsPtr = args[1];

        switch (call) {
            case (1): { // socket
                U32 *subCallArgs = memoryArrayPtr<U32>(memory, callArgsPtr, 3);
                U32 domain = subCallArgs[0];
                U32 type = subCallArgs[1];
                U32 protocol = subCallArgs[2];

                printf("SYSCALL - socket %i %i %i\n", domain, type, protocol);
                long sock = syscall(SYS_socket, domain, type, protocol);
                printf("Opened system socket %li\n", sock);

                return (I32) sock;
            }
            default: {
                printf("Unrecognised socketcall %i\n", call);
            }
        }

        return 0;
    }

    DEFINE_INTRINSIC_FUNCTION_WITH_MEM_AND_TABLE(env, "_gethostbyname", I32, _gethostbyname, I32 hostnamePtr) {
        MemoryInstance *memory = getMemoryFromRuntimeData(contextRuntimeData, defaultMemoryId.id);
        auto hostname = &memoryRef<char>(memory, (Uptr) hostnamePtr);

        printf("INTRINSIC - gethostbyname %s\n", hostname);

        return 0;
    }
    
    // ------------------------
    // Timing
    // ------------------------

    /** Struct to fake 32-bit time in wasm modules */
    struct wasm_timespec {
        I32 tv_sec;
        I32 tv_nsec;
    };

    DEFINE_INTRINSIC_FUNCTION_WITH_MEM_AND_TABLE(env, "_clock_gettime", I32, _clock_gettime, I32 clockId,
                                                 I32 resultAddress) {
        printf("INTRINSIC - _clock_gettime\n");

        // Get module's default memory
        MemoryInstance *memory = getMemoryFromRuntimeData(contextRuntimeData, defaultMemoryId.id);
        auto result = memoryRef<wasm_timespec>(memory, (Uptr) resultAddress);

        // Fake a clock incrementing by 1 with each call
        static std::atomic<U64> fakeClock;
        const U64 currentClock = fakeClock;

        result.tv_sec = I32(currentClock / 1000000000);
        result.tv_nsec = I32(currentClock % 1000000000);
        ++fakeClock;

        return 0;
    }

    // ------------------------
    // Misc
    // ------------------------

    DEFINE_INTRINSIC_FUNCTION(env, "__syscall_exit_group", I32, __syscall_exit_group, I32 a) {
        printf("SYSCALL - exit_group %i\n", a);
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__syscall_exit", I32, __syscall_exit, I32 a) {
        printf("SYSCALL - exit %i\n", a);
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__syscall_gettid", I32, __syscall_gettid, I32 a) {
        printf("SYSCALL - gettid %i\n", a);
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__syscall_tkill", I32, __syscall_tkill, I32 a, I32 b) {
        printf("SYSCALL - tkill %i %i\n", a, b);
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__syscall_rt_sigprocmask", I32, __syscall_rt_sigprocmask, I32 a, I32 b, I32 c) {
        printf("SYSCALL - rt_sigprocmask) %i %i %i\n", a, b, c);
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    // ------------------------
    // Wavix
    // ------------------------

    DEFINE_INTRINSIC_FUNCTION(env, "__wavix_get_num_args", I32, __wavix_get_num_args) {
        printf("WAVIX - get_num_args\n");
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__wavix_get_arg_length", I32, __wavix_get_arg_length, I32 a) {
        printf("WAVIX - get_arg_length\n");
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }

    DEFINE_INTRINSIC_FUNCTION(env, "__wavix_get_arg", void, __wavix_get_arg, I32 a, I32 b, I32 c) {
        printf("WAVIX - get_arg\n");
        throwException(Runtime::Exception::calledUnimplementedIntrinsicType);
    }
}