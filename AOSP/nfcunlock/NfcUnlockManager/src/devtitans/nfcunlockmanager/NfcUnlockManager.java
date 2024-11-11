package devtitans.nfcunlockmanager;

import android.util.Log;
import android.os.ServiceManager;
import android.os.IBinder;
import android.os.RemoteException;

import devtitans.nfcunlock.INfcunlock;                      // Criado pelo AIDL

public class NfcUnlockManager {
    private static final String TAG = "DevTITANS.NfcUnlockManager";
    private IBinder binder;
    private INfcunlock service;

    private static NfcUnlockManager instance;

    // Construtor. Configura a "instância da classe" (objeto) recém-criada.
    private NfcUnlockManager() {
        Log.d(TAG, "Nova (única) instância do NfcUnlockManager ...");

        binder = ServiceManager.getService("devtitans.nfcunlock.INfcunlock/default");
        if (binder != null) {
            service = INfcunlock.Stub.asInterface(binder);
            if (service != null)
                Log.d(TAG, "Serviço nfcunlock acessado com sucesso.");
            else
                Log.e(TAG, "Erro ao acessar o serviço nfcunlock!");
        }
        else
            Log.e(TAG, "Erro ao acessar o Binder!");
    }

    // Acessa a (única) instância dessa classe. Se ela não existir ainda, cria.
    public static NfcUnlockManager getInstance() {
        if (instance == null)
           instance = new NfcUnlockManager();

        return instance;
    }

    // Método que deve ser chamado para buscar o pass de forma assíncrona
    public int fetchPass() throws RemoteException {
        Log.d(TAG, "Executando método fetchPass() ...");
        int pass = service.getPass(); 
        return pass;
    }

}
