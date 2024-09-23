package devtitans.nfcunlockmanager;

import android.util.Log;
import android.os.ServiceManager;
import android.os.IBinder;
import android.os.RemoteException;

import devtitans.nfcunlock.INfcUnlock;                      // Criado pelo AIDL

public class NfcUnlockManager {
    private static final String TAG = "DevTITANS.NfcUnlockManager";
    private IBinder binder;
    private INfcUnlock service;

    private static NfcUnlockManager instance;

    // Construtor. Configura a "instância da classe" (objeto) recém-criada. 
    // Note o "private" no construtor. Essa classe só pode ser instanciada dentro desse arquivo.
    private NfcUnlockManager() {
        Log.d(TAG, "Nova (única) instância do NfcUnlockManager ...");

        binder = ServiceManager.getService("devtitans.nfcunlock.INfcUnlock/default");
        if (binder != null) {
            service = INfcUnlock.Stub.asInterface(binder);
            if (service != null)
                Log.d(TAG, "Serviço nfcunlock acessado com sucesso.");
            else
                Log.e(TAG, "Erro ao acessar o serviço nfcunlock!");
        }
        else
            Log.e(TAG, "Erro ao acessar o Binder!");
    }

    // Acessa a (única) instância dessa classe. Se ela não existir ainda, cria.
    // Note o "static" no método. Podemos executá-lo sem precisar instanciar um objeto.
    public static NfcUnlockManager getInstance() {
        if (instance == null)
            instance = new NfcUnlockManager();

        return instance;
    }

    public int connect() throws RemoteException {
        Log.d(TAG, "Executando método connect() ...");
        return service.connect();
    }

    public int getPass() throws RemoteException {
        Log.d(TAG, "Executando método getPass() ...");
        return service.getPass();
    }
}