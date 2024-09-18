#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

// Definindo o objeto NFC I2C
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

void setup() {
    Serial.begin(9600);

    // Inicializa o NFC
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("PN53x card not found!");
        while (1); // Trava o código caso não encontre o PN532
    }

    nfc.SAMConfig();
    Serial.println("Waiting for NFC card...");
}

void loop() {
    // Verifica NFC
    checkNFC();

    delay(1000); // Aguardar 1 segundo antes de verificar novamente
}

// Função para verificar o NFC
void checkNFC() {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer para armazenar o UID
    uint8_t uidLength;

    // Tenta ler um cartão NFC
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
        Serial.print("RES GET_NFC ");

        // Exibe o código lido no formato hexadecimal
        for (uint8_t i = 0; i < uidLength; i++) {
            if (uid[i] < 0x10) {
                Serial.print("0");  // Adiciona um 0 extra para formatação
            }
            Serial.print(uid[i], HEX);  // Imprime cada byte do UID em hexadecimal
        }
        Serial.println();  // Envia nova linha após a leitura
    } else {
        Serial.println("No NFC card detected.");
    }
}
