package com.example.nfc3;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.*;

import devtitans.nfcunlockmanager.NfcUnlockManager;

public class MainActivity extends AppCompatActivity{
    private static final String TAG = "DevTITANS.NfcUnlockApp";

    private TextView textStatus;
    @SuppressLint("StaticFieldLeak")
    private static TextView textPass;
    private NfcUnlockManager manager;
    int tmp;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textPass = findViewById(R.id.textPass);
        textStatus = findViewById(R.id.textStatus);

        manager = NfcUnlockManager.getInstance();
//        manager.setPassListener(this);

        updateAll(null);
    }

    public void updateAll(View view) {
        Log.d(TAG, "Atualizando dados do dispositivo ...");

        textStatus.setText("Atualizando ...");
        textStatus.setTextColor(Color.parseColor("#c47e00"));

        //manager.fetchPass();
        new  FetchTask().execute();

    }

    //@Override
    public static void onPassRead(int pass) {
        Log.d(TAG, "Pass recebido: " + pass);
        textPass.setText(String.valueOf(pass));
/*
        try {
            int status = manager.connect();
            if (status == 0) {
                textStatus.setText("Desconectado");
                textStatus.setTextColor(Color.parseColor("#73312f"));
            } else if (status == 1) {
                textStatus.setText("Conectado");
                textStatus.setTextColor(Color.parseColor("#6d790c"));
            }
        } catch (RemoteException e) {
            Toast.makeText(this, "Erro ao acessar o Binder!", Toast.LENGTH_LONG).show();
            Log.e(TAG, "Erro atualizando status da conexão:", e);
        } */
    }
    class FetchTask extends AsyncTask<Integer, Void, Void> {
        @Override
        protected Void doInBackground(Integer... params) {

            NfcUnlockManager manager;
            manager = NfcUnlockManager.getInstance();

            try {
                tmp = manager.fetchPass();
            } catch (RemoteException e) {
                throw new RuntimeException(e);
            }
            //MainActivity.onPassRead(tmp);
            onPassRead(tmp);
            return null;
        }
    }
}


