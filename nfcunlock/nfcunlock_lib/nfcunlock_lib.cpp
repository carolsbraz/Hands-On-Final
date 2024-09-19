#include "nfcunlock_lib.h"
#include <unistd.h>

using namespace std;                                   // Permite usar string, ifstream diretamente ao invés de std::string
using namespace android::base;                         // Permite usar GetBoolProperty ao invés de android::base::GetBoolProperty


namespace devtitans::nfcunlock {                       // Entra no pacote devtitans::nfcunlock


int Nfcunlock::connect() {
   char dirPath[] = "/sys/kernel/nfcunlock";
   struct stat dirStat;
   if (stat(dirPath, &dirStat) == 0)
       if (S_ISDIR(dirStat.st_mode))
           return 1;                                  // Se o diretório existir, retorna 1


   // Diretório não existe, vamos verificar a propriedade
   //bool allowSimulated = GetBoolProperty("devtitans.nfcunlock.allow_simulated", true);
   //if (!allowSimulated)
   return 0;                                      // Dispositivo não encontrado
}


int Nfcunlock::readFileValue(string file) {
   int connected = this->connect();


   if (connected == 1) {                          // Conectado. Vamos solicitar o valor ao dispositivo
       int value;
       string filename = string("/sys/kernel/nfcunlock/") + file;
       ifstream file(filename);                        // Abre o arquivo do módulo do kernel


       if (file.is_open()) {                           // Verifica se o arquivo foi aberto com sucesso
           file >> value;                              // Lê um inteiro do arquivo
           file.close();
           return value;
       }
   }


   // Se chegou aqui, não foi possível conectar ou se comunicar com o dispositivo
   return -1;
}


bool Nfcunlock::writeFileValue(string file, int value) {
   int connected = this->connect();


   if (connected == 1) {                          // Conectado. Vamos solicitar o valor ao dispositivo
       string filename = string("/sys/kernel/nfcunlock/") + file;
       ofstream file(filename, ios::trunc);            // Abre o arquivo limpando o seu conteúdo


       if (file.is_open()) {                           // Verifica se o arquivo foi aberto com sucesso
           file << value;                              // Escreve o trigger no arquivo
           file.close();
           return true;
       }
   }


   // Se chegou aqui, não foi possível conectar ou se comunicar com o dispositivo
   return false;
}


int Nfcunlock::getPass() {
   if(this->setTrigg(1)){
    sleep(30.0);
    return this->readFileValue("pass");
   }

    return 0;
}


bool Nfcunlock::setTrigg(int trigger) {
   return this->writeFileValue("trigg", trigger);
}


} // namespace
