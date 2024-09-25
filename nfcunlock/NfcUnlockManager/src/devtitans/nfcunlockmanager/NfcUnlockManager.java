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
    private PassListener passListener;                      // Listener para o pass

    private static NfcUnlockManager instance;

    // Construtor. Configura a "instância da classe" (objeto) recém-criada.
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
    public static NfcUnlockManager getInstance() {
        if (instance == null)
            instance = new NfcUnlockManager();

        return instance;
    }

    // Método para registrar o listener que será chamado quando o pass for lido
    public void setPassListener(PassListener listener) {
        this.passListener = listener;
    }

    // Método chamado para notificar o listener quando o pass for lido
    private void notifyPassListener(int pass) {
        if (passListener != null) {
            passListener.onPassRead(pass);
        }
    }

    // Método que deve ser chamado para buscar o pass de forma assíncrona
    public void fetchPass() throws RemoteException {
        Log.d(TAG, "Executando método fetchPass() ...");
        int pass = service.getPass();  // Busca o pass do serviço
        notifyPassListener(pass);      // Notifica o listener
    }

    public int connect() throws RemoteException {
        Log.d(TAG, "Executando método connect() ...");
        return service.connect();
    }

    // Interface do listener que será implementada pela Activity
    public interface PassListener {
        void onPassRead(int pass);
    }
}
