# Herda as configurações do emulador (produto sdk_phone_x86_64)
$(call inherit-product, $(SRC_TARGET_DIR)/product/sdk_phone_x86_64.mk)

# Sobrescreve algumas variáveis com os dados do novo produto
PRODUCT_NAME := devtitans_nfcunlock
PRODUCT_DEVICE := nfcunlock
PRODUCT_BRAND := NFCLCKBrand
PRODUCT_MODEL := NFCLCKModel