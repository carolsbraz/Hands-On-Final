#include <SPI.h>
#include <Adafruit_PN532.h>

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);


bool readNFCFlag = false;      // Variável de controle para ativar/desativar a leitura NFC
unsigned long startTime = 0;   // Armazena o tempo de início da espera de leitura NFC
const unsigned long timeout = 30000; // Tempo limite de 30 segundos (em milissegundos)

void setup() {
    Serial.begin(9600);

    // Inicializa o NFC
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        while (1); // Trava o código caso não encontre o PN532
    }

    //nfc.SAMConfig();
}

void loop() {
    // Verifica comandos via serial
    if (Serial.available() > 0) {
        String serialCommand = Serial.readStringUntil('\n');
        serialCommand.trim();
        serialCommand.toUpperCase();

        if (serialCommand == "GET_PASS") {
            readNFCFlag = true; // Ativa a leitura do NFC
            startTime = millis(); // Marca o tempo de início
        } else {
            Serial.println("ERR Unknown command.");
        }
    }

    // Se o comando "GET_PASS" foi recebido, realiza a leitura do NFC
    if (readNFCFlag) {
        if (millis() - startTime <= timeout) { // Verifica se ainda está dentro do tempo limite
            checkNFC(); // Tenta ler o NFC
        } else {
            readNFCFlag = false; // Desativa a leitura após o timeout
            Serial.println("Timeout: No NFC card detected.");
        }
    }

    delay(100);
}

// Função para verificar o NFC e enviar os primeiros 4 dígitos como "PASS"
void checkNFC() {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer para armazenar o UID (não será usado aqui)
    uint8_t uidLength;

    // Tenta ler um cartão NFC
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
        // Autentica e lê o bloco 4 da memória
        uint8_t blockNumber = 4; // Bloco onde os dados foram gravados
        uint8_t data[16]; // Buffer para armazenar os dados lidos do bloco

        if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockNumber, 0, (uint8_t*)"\xFF\xFF\xFF\xFF\xFF\xFF")) {
            
            if (nfc.mifareclassic_ReadDataBlock(blockNumber, data)) {
                // concatena os 4 primeiros bytes em uma string de inteiros e imprime no formato desejado
                String passCode = "";
                for (int i = 0; i < 4; i++) {
                    passCode += String(data[i]); // Concatena cada byte como string
                }

                // Limita a string resultante a 4 dígitos
                if (passCode.length() > 4) {
                    passCode = passCode.substring(0, 4); // Pegar somente os 4 primeiros digitos
                }

                // Imprime o código com "PASS" e os primeiros 4 dígitos
                Serial.println("PASS " + passCode + "\n");

                // Desativa a leitura do NFC após o sucesso
                readNFCFlag = false;
            }
        }
    }
}
