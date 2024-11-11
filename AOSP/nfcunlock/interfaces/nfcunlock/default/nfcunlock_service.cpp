#include "nfcunlock_service.h"

namespace aidl::devtitans::nfcunlock {
    ndk::ScopedAStatus NfcunlockService::getPass(int32_t* _aidl_return) {
        *_aidl_return = this->nfcunlock.getPass();
        LOG(INFO) << "getPass(): " << *_aidl_return;
        return ndk::ScopedAStatus::ok();
    }

}
