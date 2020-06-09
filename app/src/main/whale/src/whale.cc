#include <dlfcn.h>
#include "whale.h"
#include "interceptor.h"
#include "dbi/instruction_set.h"

#define __uintval(p)               reinterpret_cast<uintptr_t>(p)
#define __ptr(p)                   reinterpret_cast<void *>(p)
#define __align_up(x, n)           (((x) + ((n) - 1)) & ~((n) - 1))
#define __align_down(x, n)         ((x) & -(n))
#define __page_size                4096
#define __page_align(n)            __align_up(static_cast<uintptr_t>(n), __page_size)
#define __ptr_align(x)             __ptr(__align_down(reinterpret_cast<uintptr_t>(x), __page_size))
#define __make_rwx(p, n)           ::mprotect(__ptr_align(p), \
                                              __page_align(__uintval(p) + n) != __page_align(__uintval(p)) ? __page_align(n) + __page_size : __page_align(n), \
                                              PROT_READ | PROT_WRITE | PROT_EXEC)

#if defined(__arm__)
#include "dbi/arm/inline_hook_arm.h"
#elif defined(__aarch64__)

#include "dbi/arm64/inline_hook_arm64.h"

#elif defined(__i386__)
#include "dbi/x86/inline_hook_x86.h"
#elif defined(__x86_64__)

#include "dbi/x86_64/inline_hook_x86_64.h"

#endif

#if defined(__APPLE__)

#include "dbi/darwin/macho_import_hook.h"

#endif

#if defined(linux)

#include "platform/linux/elf_image.h"
#include "platform/linux/process_map.h"

#endif


OPEN_API void WInlineHookFunction(void *address, void *replace, void **backup) {
    __make_rwx(address, __page_size);
#if defined(__arm__)
    std::unique_ptr<whale::Hook> hook(
            new whale::arm::ArmInlineHook(
                    reinterpret_cast<intptr_t>(address),
                    reinterpret_cast<intptr_t>(replace),
                    reinterpret_cast<intptr_t *>(backup)
            )
    );
    whale::Interceptor::Instance()->AddHook(hook);
#elif defined(__aarch64__)
    std::unique_ptr<whale::Hook> hook(
            new whale::arm64::Arm64InlineHook(
                    reinterpret_cast<intptr_t>(address),
                    reinterpret_cast<intptr_t>(replace),
                    reinterpret_cast<intptr_t *>(backup)
            )
    );
    whale::Interceptor::Instance()->AddHook(hook);
#elif defined(__i386__)
    std::unique_ptr<whale::Hook> hook(
            new whale::x86::X86InlineHook(
                    reinterpret_cast<intptr_t>(address),
                    reinterpret_cast<intptr_t>(replace),
                    reinterpret_cast<intptr_t *>(backup)
            )
    );
    whale::Interceptor::Instance()->AddHook(hook);
#elif defined(__x86_64__)
    std::unique_ptr<whale::Hook> hook(
            new whale::x86_64::X86_64InlineHook(
                    reinterpret_cast<intptr_t>(address),
                    reinterpret_cast<intptr_t>(replace),
                    reinterpret_cast<intptr_t *>(backup)
            )
    );
    whale::Interceptor::Instance()->AddHook(hook);
#else
    LOG(WARNING) << "Unsupported ISA to Hook Function: " << whale::kRuntimeISA;
#endif
}

OPEN_API void WImportHookFunction(const char *name, const char *libname, void *replace, void **backup) {
#if defined(__APPLE__)
    std::unique_ptr<whale::Hook> hook(new whale::darwin::MachoImportHook(
            name,
            replace,
            backup
    ));
    whale::Interceptor::Instance()->AddHook(hook);
#endif
}

OPEN_API void *WDynamicLibOpen(const char *name) {
#ifdef linux
    auto range = whale::FindExecuteMemoryRange(name);
    if (!range->IsValid()) {
        return nullptr;
    }
    whale::ElfImage *image = new whale::ElfImage();
    if (!image->Open(range->path_, range->base_)) {
        delete image;
        return nullptr;
    }
    return reinterpret_cast<void *>(image);
#else
    return dlopen(name, RTLD_NOW);
#endif
}

OPEN_API void *WDynamicLibOpenAlias(const char *name, const char *path) {
#ifdef linux
    auto range = whale::FindExecuteMemoryRange(name);
    if (!range->IsValid()) {
        return nullptr;
    }
    whale::ElfImage *image = new whale::ElfImage();
    if (!image->Open(path, range->base_)) {
        delete image;
        return nullptr;
    }
    return reinterpret_cast<void *>(image);
#else
    return dlopen(name, RTLD_NOW);
#endif
}

OPEN_API void *WDynamicLibSymbol(void *handle, const char *name) {
    if (handle == nullptr || name == nullptr) {
        return nullptr;
    }
#ifdef linux
    whale::ElfImage *image = reinterpret_cast<whale::ElfImage *>(handle);
    return image->FindSymbol<void *>(name);
#else
    return dlsym(handle, name);
#endif
}

OPEN_API void WDynamicLibClose(void *handle) {
#ifdef linux
    whale::ElfImage *image = reinterpret_cast<whale::ElfImage *>(handle);
        delete image;
#else
    dlclose(handle);
#endif
}
