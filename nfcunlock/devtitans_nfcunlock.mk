# Herda as configurações do emulador (produto sdk_phone_x86_64)
$(call inherit-product, $(SRC_TARGET_DIR)/product/sdk_phone_x86_64.mk)

# Sobrescreve algumas variáveis com os dados do novo produto
PRODUCT_NAME := devtitans_nfcunlock
PRODUCT_DEVICE := nfcunlock
PRODUCT_BRAND := NFCLCKBrand
PRODUCT_MODEL := NFCLCKModel

# Smartlamp AIDL Interface
PRODUCT_PACKAGES += devtitans.nfcunlock

# Smartlamp Binder Service
PRODUCT_PACKAGES += devtitans.nfcunlock-service

# Device Framework Matrix (Declara que o nosso produto Kraken precisa do serviço smartlamp)
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE := device/devtitans/nfcunlock/device_framework_matrix.xml

BOARD_SEPOLICY_DIRS += device/devtitans/nfcunlock/sepolicy

# Manager
PRODUCT_PACKAGES += devtitans.nfcunlockmanager

# Cliente de Linha de Comando para o Serviço Smartlamp
PRODUCT_PACKAGES += nfcunlock_service_client
