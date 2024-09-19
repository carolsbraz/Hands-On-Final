#pragma once

#include <android-base/logging.h>
#include <android/binder_process.h>
#include <android/binder_manager.h>

#include <aidl/devtitans/nfcunlock/BnNfcunlock.h>            // Criado pelo AIDL automaticamente

#include "nfcunlock_lib.h"                                   // Classe Smartlamp (biblioteca)

using namespace devtitans::nfcunlock;

namespace aidl::devtitans::nfcunlock {

class NfcunlockService : public BnNfcunlock {
    public:
        ndk::ScopedAStatus getPass(int32_t* _aidl_return) override;

    private:
        Nfcunlock nfcunlock;                                 // Biblioteca
};

}
