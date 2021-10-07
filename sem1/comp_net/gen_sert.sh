#!/bin/sh

fio='shushuevai'
group='msp99'
email='statiz.arm@gmail.com'

rm -f $fio-$group*

# Корневой сертификат
# ключ
openssl genrsa -out $fio-$group-ca.key -aes256 -passout pass:$fio 4096

# Сертификат
C='RU'
ST='Moscow'
L='Moscow'
O=$fio
OU="${fio} P1"
CN="${fio} CA"
printf "${C}\n${ST}\n${L}\n${O}\n${OU}\n${CN}\n${email}\n" | \
openssl req \
    -config openssl-ca.conf -new -x509 -days 1825 -key $fio-$group-ca.key -out \
    $fio-$group-ca.crt -passin pass:$fio

openssl x509 -in $fio-$group-ca.crt -out $fio-$group-ca.crt \
    -passin pass:$fio

# Промежуточный сертификат
# Ключ
openssl genrsa -out $fio-$group-intr.key -aes256 -passout pass:$fio 4096

# Сертификат
C='RU'
ST='Moscow'
L='Moscow'
O=$fio
OU="${fio} P1"
CN="${fio} Intermediate CA"
printf "${C}\n${ST}\n${L}\n${O}\n${OU}\n${CN}\n${email}\n\n\n" | \
openssl req \
    -config openssl-intr.conf -new -key $fio-$group-intr.key -out \
    $fio-$group-intr.crt -passin pass:$fio

openssl x509 -req -days 365 \
    -extensions v3_ca -extfile openssl-intr.conf \
    -CA $fio-$group-ca.crt \
    -CAkey $fio-$group-ca.key \
    -CAcreateserial \
    -CAserial serial \
    -in $fio-$group-intr.crt -out $fio-$group-intr.pem \
    -passin pass:$fio

mv $fio-$group-intr.pem $fio-$group-intr.crt

# Базовый сертификат
# Ключ
openssl genrsa -out $fio-$group-basic.key -passout pass: 2048

# Сертификат
C='RU'
ST='Moscow'
L='Moscow'
O=$fio
OU="${fio} P1"
CN="${fio} Basic"
printf "${C}\n${ST}\n${L}\n${O}\n${OU}\n${CN}\n${email}\n\n\n" | \
openssl req \
    -config openssl-basic.conf -new -key $fio-$group-basic.key -out \
    $fio-$group-basic.crt -passin pass:

openssl x509 -req -days 365 \
    -extensions v3_ca -extfile openssl-basic.conf \
    -CA $fio-$group-intr.crt \
    -CAkey $fio-$group-intr.key \
    -CAcreateserial \
    -CAserial serial \
    -in $fio-$group-basic.crt -out $fio-$group-basic.crt \
    -passin pass:$fio

zip $fio-$group-p1_1.zip *.crt *.key

