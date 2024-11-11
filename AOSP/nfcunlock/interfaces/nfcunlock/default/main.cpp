#include "nfcunlock_service.h"

using namespace aidl::devtitans::nfcunlock;                // SmartlampService (smartlamp_service.cpp)
using namespace std;                                       // std::shared_ptr
using namespace ndk;                                       // ndk::SharedRefBase

int main() {
    LOG(INFO) << "Iniciando Nfcunlock AIDL Service ...";

    ABinderProcess_setThreadPoolMaxThreadCount(0);

    shared_ptr<NfcunlockService> nfcunlock_service = SharedRefBase::make<NfcunlockService>();

    const string instance = std::string() + INfcunlock::descriptor + "/default";   // devtitans.smartlamp.ISmartlamp/default
    binder_status_t status = AServiceManager_addService(nfcunlock_service->asBinder().get(), instance.c_str());
    CHECK(status == STATUS_OK);

    LOG(INFO) << "Nfcunlock AIDL Service iniciado com nome: " << instance;
    ABinderProcess_joinThreadPool();

    return EXIT_FAILURE;                                   // Não deve chegar nunca aqui
}
