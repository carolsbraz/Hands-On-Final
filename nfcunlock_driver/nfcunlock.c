#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>

MODULE_AUTHOR("DevTITANS <devtitans@icomp.ufam.edu.br>");
MODULE_DESCRIPTION("Driver de acesso ao NFC_Unlock (ESP32 com Chip Serial CP2102");
MODULE_LICENSE("GPL");

#define MAX_RECV_LINE 100 // Tamanho máximo de uma linha de resposta do dispositivo USB

static char recv_line[MAX_RECV_LINE];              // Armazena dados vindos da USB até receber um caractere de nova linha '\n'
static struct usb_device *nfcunlock_device;        // Referência para o dispositivo USB
static uint usb_in, usb_out;                       // Endereços das portas de entrada e saída da USB
static char *usb_in_buffer, *usb_out_buffer;       // Buffers de entrada e saída da USB
static int usb_max_size;                           // Tamanho máximo de uma mensagem USB

#define VENDOR_ID   0x10C4  // VendorID do nfcunlock
#define PRODUCT_ID  0XEA60  // ProductID do nfcunlock
static const struct usb_device_id id_table[] = { { USB_DEVICE(VENDOR_ID, PRODUCT_ID) }, {} };

static int usb_probe(struct usb_interface *ifce, const struct usb_device_id *id); // Executado quando o dispositivo é conectado na USB
static void usb_disconnect(struct usb_interface *ifce);                           // Executado quando o dispositivo USB é desconectado da USB
static int usb_read_serial(void);   
static int usb_write_serial(char *cmd, int param);
static int usb_send_cmd(char *cmd, int param);

// Executado quando o arquivo /sys/kernel/nfcunlock/trigg é lido
static ssize_t attr_show(struct kobject *sys_obj, struct kobj_attribute *attr, char *buff);
// Executado quando o arquivo /sys/kernel/nfcunlock/pass é escrito
static ssize_t attr_store(struct kobject *sys_obj, struct kobj_attribute *attr, const char *buff, size_t count);

// Variáveis para criar os arquivos no /sys/kernel/nfcunlock/{trigg, pass}
static struct kobj_attribute pass_attribute = __ATTR(pass, S_IRUGO | S_IWUSR, attr_show, attr_store);
static struct attribute *attrs[] = { &pass_attribute.attr, NULL };
static struct attribute_group attr_group = { .attrs = attrs };
static struct kobject *sys_obj; // Executado para ler a saída da porta serial

MODULE_DEVICE_TABLE(usb, id_table);
bool ignore = true;

static struct usb_driver nfcunlock_driver = {
    .name        = "nfcunlock",     // Nome do driver
    .probe       = usb_probe,       // Executado quando o dispositivo é conectado na USB
    .disconnect  = usb_disconnect,  // Executado quando o dispositivo é desconectado na USB
    .id_table    = id_table,        // Tabela com o VendorID e ProductID do dispositivo
};

module_usb_driver(nfcunlock_driver);

static int usb_write_serial(char *cmd, int param) {
    int ret, actual_size;
    char resp_expected[MAX_RECV_LINE]; // Resposta esperada do comando

    // Armazena o comando em formato de texto que o firmware reconheça
    snprintf(usb_out_buffer, usb_max_size, "%s\n", cmd);

    // Grave o valor de usb_out_buffer com printk
    printk(KERN_INFO "NFC_Unlock: Enviando comando: %s\n", usb_out_buffer);

    // Envia o comando pela porta Serial
    ret = usb_bulk_msg(nfcunlock_device, usb_sndbulkpipe(nfcunlock_device, usb_out), usb_out_buffer, strlen(usb_out_buffer), &actual_size, 1000*HZ);
    if (ret) {
        printk(KERN_ERR "NFC_Unlock: Erro de código %d ao enviar comando!\n", ret);
        return -1;
    }

    return 0;
}

static int usb_read_serial() {
    int recv_size = 0;                      // Quantidade de caracteres no recv_line
    int ret, actual_size;
    int retries = 10;  // Tenta algumas vezes receber uma resposta da USB. Depois desiste.
    int key = 0;
    char *start_ptr;
    char resp_expected[MAX_RECV_LINE];      // Resposta esperada do comando
    char *resp_pos;                         // Posição na linha lida que contém o número retornado pelo dispositivo
    long resp_number = -1;                  // Número retornado pelo dispositivo (e.g., valor do led, valor do ldr)
    //printk("LENDO SERIAL");
    // Espera pela resposta correta do dispositivo (desiste depois de várias tentativas)
    while (retries > 0) {
        // Lê os dados da porta serial e armazena em usb_in_buffer
        ret = usb_bulk_msg(nfcunlock_device, usb_rcvbulkpipe(nfcunlock_device, usb_in), usb_in_buffer, usb_max_size, &actual_size, 1000*HZ);
        if (ret) {
            printk(KERN_ERR "NFC_Unlock: Erro ao ler dados da USB (tentativa %d). Código: %d\n", retries, ret);
            retries--;
            continue;
        }

        // Para cada caractere recebido ...
        sprintf(resp_expected, "PASS");
        for (int i=0; i<actual_size; i++) {

            if (usb_in_buffer[i] == '\n') {  // Temos uma linha completa
                recv_line[recv_size] = '\0'; // Encerra a string corretamente
                printk(KERN_INFO "NFC_Unlock: Recebido uma linha: '%s'\n", recv_line);

                // Verifica se o início da linha recebida é igual à resposta esperada do comando enviado
                if (!strncmp(recv_line, resp_expected, strlen(resp_expected))) {
                    printk(KERN_INFO "NFC_Unlock: Linha eh resposta para %s! Processando ...\n", resp_expected);

                    // Acessa a parte da resposta que contém o número e converte para inteiro
                    resp_pos = &recv_line[strlen(resp_expected) + 1];
                    ignore = kstrtol(resp_pos, 10, &resp_number);  // AQUI

                    return resp_number;
                }
                else { // Não é a linha que estávamos esperando. Pega a próxima.
                    printk(KERN_INFO "NFC_Unlock: Nao eh resposta para %s! Tentiva %d. Proxima linha...\n", resp_expected, retries--);
                    recv_size = 0; // Limpa a linha lida (recv_line)
                }
            }
            else { // É um caractere normal (sem ser nova linha), coloca no recv_line e lê o próximo caractere
                recv_line[recv_size] = usb_in_buffer[i];
                recv_size++;
            }
        }
    }

    printk(KERN_ERR "NFC_Unlock: Falha em receber uma resposta válida após várias tentativas.\n");
    return -1;  // Retorna erro se não conseguir extrair o valor após várias tentativas
}

// Executado quando o dispositivo é conectado na USB
static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct usb_endpoint_descriptor *usb_endpoint_in, *usb_endpoint_out;

    printk(KERN_INFO "NFC_Unlock: Dispositivo conectado ...\n");

    // Cria arquivos do /sys/kernel/nfcunlock/*
    sys_obj = kobject_create_and_add("nfcunlock", kernel_kobj);
    if (!sys_obj) {
        printk(KERN_ERR "NFC_Unlock: Falha ao criar kobject.\n");
        return -ENOMEM;
    }
    if (sysfs_create_group(sys_obj, &attr_group)) {
        kobject_put(sys_obj);
        printk(KERN_ERR "NFC_Unlock: Falha ao criar sysfs group.\n");
        return -ENOMEM;
    }

    // Detecta portas e aloca buffers de entrada e saída de dados na USB
    nfcunlock_device = interface_to_usbdev(interface);
    if (usb_find_common_endpoints(interface->cur_altsetting, &usb_endpoint_in, &usb_endpoint_out, NULL, NULL)) {
        printk(KERN_ERR "NFC_Unlock: Falha ao encontrar endpoints.\n");
        return -ENODEV;
    }
    usb_max_size = usb_endpoint_maxp(usb_endpoint_in);
    usb_in = usb_endpoint_in->bEndpointAddress;
    usb_out = usb_endpoint_out->bEndpointAddress;
    usb_in_buffer = kmalloc(usb_max_size, GFP_KERNEL);
    usb_out_buffer = kmalloc(usb_max_size, GFP_KERNEL);

    if (!usb_in_buffer || !usb_out_buffer) {
        printk(KERN_ERR "NFC_Unlock: Falha ao alocar buffers USB.\n");
        return -ENOMEM;
    }

    return 0;
}

// Executado quando o dispositivo USB é desconectado da USB
static void usb_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "NFC_Unlock: Dispositivo desconectado.\n");
    kfree(usb_in_buffer);  // Desaloca buffers
    kfree(usb_out_buffer);
    kobject_put(sys_obj);
}

// Envia um comando via USB, espera e retorna a resposta do dispositivo (convertido para int)
static int usb_send_cmd(char *cmd, int param) {
    int result = usb_write_serial(cmd, param);
    if (result < 0) {
        return result;
    }
    return usb_read_serial();
}

// Executado quando o arquivo /sys/kernel/nfcunlock/trigg é lido
static ssize_t attr_show(struct kobject *sys_obj, struct kobj_attribute *attr, char *buff) {
    int value = -1;
    const char *attr_name = attr->attr.name;

    printk(KERN_INFO "NFC_Unlock: Lendo %s ...\n", attr_name);

    value = usb_send_cmd("GET_PASS", -1);

    snprintf(buff, PAGE_SIZE, "%d\n", value);
    return strlen(buff);
}

// Executado quando o arquivo /sys/kernel/nfcunlock/pass é escrito
static ssize_t attr_store(struct kobject *sys_obj, struct kobj_attribute *attr, const char *buff, size_t count) {
    long value;
    const char *attr_name = attr->attr.name;
    int result;

    result = usb_send_cmd("GET_PASS", 0);
    if (result < 0) {
        printk(KERN_ALERT "NFC_Unlock: erro ao setar o valor do %s.\n", attr_name);
        return result;
    }

    if (strcmp(attr_name, "pass") == 0) {
        printk(KERN_ERR "NFC_Unlock: Não é possível escrever no arquivo pass\n");
        return -EACCES;
    } 

    if (kstrtol(buff, 10, &value)) {
        printk(KERN_ALERT "NFC_Unlock: valor de %s inválido.\n", attr_name);
        return -EINVAL;
    }

    printk(KERN_INFO "NFC_Unlock: Setando %s para %ld ...\n", attr_name, value);

    return count;
}
