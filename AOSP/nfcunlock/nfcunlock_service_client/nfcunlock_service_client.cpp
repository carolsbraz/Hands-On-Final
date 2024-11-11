#include <android/binder_manager.h>
#include <aidl/devtitans/nfcunlock/INfcunlock.h>
#include <iostream>                             // std::cout e std::endl (end-line)

using namespace aidl::devtitans::nfcunlock;     // ISmartlamp
using namespace std;                            // std::shared_ptr
using namespace ndk;                            // ndk::SpAIBinder

int main() {
    shared_ptr<INfcunlock> service;
    service = INfcunlock::fromBinder(SpAIBinder(AServiceManager_getService("devtitans.nfcunlock.INfcunlock/default")));
    
    if (!service) {
        cout << "Erro acessando o serviço!" << endl;
        return 1;
    }

    int32_t _aidl_return;
    ScopedAStatus status = service->getPass(&_aidl_return);
    int pass = static_cast<int32_t>(_aidl_return);
    cout << "Pass: " << pass << endl;
    
    return 0;
}
