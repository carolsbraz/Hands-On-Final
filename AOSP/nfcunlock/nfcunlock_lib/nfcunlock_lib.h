#pragma once                           // Inclui esse cabeçalho apenas uma vez


#include <fstream>                     // Classe ifstream
#include <android-base/properties.h>   // Função GetBoolProperty
#include <sys/stat.h>                  // Função e struct stat
#include <random>                      // Geração de números aleatórios (valores simulados)


using namespace std;                   // Permite usar string diretamente ao invés de std::string


namespace devtitans::nfcunlock {       // Pacote Smartlamp


class Nfcunlock {
   public:
       /**
        * Verifica se o diretório /sys/kernel/nfcunlock existe. Se existir
        * o dispositivo SmartLamp está conectado via USB. Caso contrário,
        * verifica a propriedade devtitans.smartlamp.allow_simulated
        * para ver se valores simulados podem ser usados.
        *
        * Retorna:
        *      0: dispositivo não encontrado
        *      1: sucesso
        */
       int connect();


       /**
        * Inicia a leitura do cartão.
        *
        * Seta o trigger para 1 avisando que deve iniciar a leitura,
        * então espera por 30 segundos e lê o valor no arquivo /pass
        *
        */
       int getPass();


       /**
        * Trigg - Trigger para inciar leitura.
        * Ao executar esse método, o trigger é disparado
        * no arquivo /trigg para avisar o driver se
        * que está se esperando a leitura do cartão
        *
        * Valor retornado/setado:
        *      1: iniciar leitura
        */
       bool setTrigg(int trigg);


   private:
       /**
        * Métodos para ler e escrever valores nos arquivos "led",
        * "ldr" ou "threshold" do diretório /sys/kernel/nfcunlock.
        */
       int readFileValue(string file);
       bool writeFileValue(string file, int value);
};


} // namespace
