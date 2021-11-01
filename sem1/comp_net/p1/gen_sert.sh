#!/bin/sh

fio='shushuevai'
group='msp21'
email='statiz.arm@gmail.com'

rm -f $fio-*

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

openssl x509 -req -days 90 \
    -extensions v3_ca -extfile openssl-basic.conf \
    -CA $fio-$group-intr.crt \
    -CAkey $fio-$group-intr.key \
    -CAcreateserial \
    -CAserial serial \
    -in $fio-$group-basic.crt -out $fio-$group-basic.crt \
    -passin pass:$fio

zip $fio-$group-p1_1.zip $fio-$group-basic.*  $fio-$group-intr.* $fio-$group-ca.*

# Валидный сертификат
# Ключ
openssl genrsa -out $fio-$group-crl-valid.key -passout pass: 2048

# Сертификат
C='RU'
ST='Moscow'
L='Moscow'
O=$fio
OU="${fio} P2"
CN="${fio} CRL Valid"
printf "${C}\n${ST}\n${L}\n${O}\n${OU}\n${CN}\n${email}\n\n\n" | \
openssl req \
    -config openssl-crl-valid.conf -new -key $fio-$group-crl-valid.key -out \
    $fio-$group-crl-valid.crt -passin pass:

openssl x509 -req -days 90 \
    -extensions v3_ca -extfile openssl-crl-valid.conf \
    -CA $fio-$group-intr.crt \
    -CAkey $fio-$group-intr.key \
    -CAcreateserial \
    -CAserial serial \
    -in $fio-$group-crl-valid.crt -out $fio-$group-crl-valid.crt \
    -passin pass:$fio

# Отозванный сертификат
# Ключ
openssl genrsa -out $fio-$group-crl-revoked.key -passout pass: 2048

# Сертификат
C='RU'
ST='Moscow'
L='Moscow'
O=$fio
OU="${fio} P2"
CN="${fio} CRL Revoked"
printf "${C}\n${ST}\n${L}\n${O}\n${OU}\n${CN}\n${email}\n\n\n" | \
openssl req \
    -config openssl-crl-revoked.conf -new -key $fio-$group-crl-revoked.key -out \
    $fio-$group-crl-revoked.crt -passin pass:

openssl x509 -req -days 90 \
    -extensions v3_ca -extfile openssl-crl-revoked.conf \
    -CA $fio-$group-intr.crt \
    -CAkey $fio-$group-intr.key \
    -CAcreateserial \
    -CAserial serial \
    -in $fio-$group-crl-revoked.crt -out $fio-$group-crl-revoked.crt \
    -passin pass:$fio

# Цепочка
# директория для файлов ca
mkdir -p cadir
# index.txt как бд для хранения списков отозванных
rm cadir/index.txt
touch  cadir/index.txt
echo 1000 > cadir/crlnumber

# Отзываем сертификат
openssl ca -config openssl-intr.conf \
           -keyfile shushuevai-msp21-intr.key \
           -cert shushuevai-msp21-intr.crt \
           -revoke shushuevai-msp21-crl-revoked.crt \
           -passin pass:shushuevai

# Генерируем список отозванных сертификатов
openssl ca -config openssl-intr.conf \
           -keyfile $fio-$group-intr.key \
           -cert $fio-$group-intr.crt \
           -gencrl -out $fio-$group.crl \
           -crlexts usr_cert \
           -passin pass:shushuevai

# Генерируем цепочку сертификатов
cat $fio-$group-intr.crt $fio-$group-ca.crt > $fio-$group-chain.crt

# Собираем архив
zip $fio-$group-p1_2.zip $fio-$group-crl-valid.*  $fio-$group-crl-revoked.* \
    $fio-$group.crl $fio-$group-chain.crt

# Сертификат OCSP
# Ключ
openssl genrsa -out $fio-$group-ocsp-resp.key -aes256 -passout pass:$fio 4096

# Сертификат
C='RU'
ST='Moscow'
L='Moscow'
O=$fio
OU="${fio} P3"
CN="${fio} OCSP Responder"
printf "${C}\n${ST}\n${L}\n${O}\n${OU}\n${CN}\n${email}\n\n\n" | \
openssl req \
    -config openssl-ocsp-resp.conf -new -key $fio-$group-ocsp-resp.key -out \
    $fio-$group-ocsp-resp.crt -passin pass:$fio

openssl x509 -req -days 365 \
    -extensions v3_ca -extfile openssl-ocsp-resp.conf \
    -CA $fio-$group-intr.crt \
    -CAkey $fio-$group-intr.key \
    -CAcreateserial \
    -CAserial serial \
    -in $fio-$group-ocsp-resp.crt -out $fio-$group-ocsp-resp.crt \
    -passin pass:$fio

# Цепочка
cat $fio-$group-ocsp-resp.crt $fio-$group-chain.crt > $fio-$group-chain.crt.new
mv $fio-$group-chain.crt.new $fio-$group-chain.crt

# OCSP Валидный сертификат
# Ключ
openssl genrsa -out $fio-$group-ocsp-valid.key -passout pass: 2048

# Сертификат
C='RU'
ST='Moscow'
L='Moscow'
O=$fio
OU="${fio} P3"
CN="${fio} OCSP Valid"
printf "${C}\n${ST}\n${L}\n${O}\n${OU}\n${CN}\n${email}\n\n\n" | \
openssl req \
    -config openssl-ocsp-valid.conf -new -key $fio-$group-ocsp-valid.key -out \
    $fio-$group-ocsp-valid.crt -passin pass:

openssl x509 -req -days 90 \
    -extensions v3_ca -extfile openssl-ocsp-valid.conf \
    -CA $fio-$group-intr.crt \
    -CAkey $fio-$group-intr.key \
    -CAcreateserial \
    -CAserial serial \
    -in $fio-$group-ocsp-valid.crt -out $fio-$group-ocsp-valid.crt \
    -passin pass:$fio

# Валидируем сертификат
openssl ca -config openssl-intr.conf \
           -keyfile shushuevai-msp21-intr.key \
           -cert shushuevai-msp21-intr.crt \
           -valid shushuevai-msp21-ocsp-valid.crt \
           -passin pass:shushuevai

# OCSP Отозванный сертификат
# Ключ
openssl genrsa -out $fio-$group-ocsp-revoked.key -passout pass: 2048

# Сертификат
C='RU'
ST='Moscow'
L='Moscow'
O=$fio
OU="${fio} P3"
CN="${fio} OCSP Revoked"
printf "${C}\n${ST}\n${L}\n${O}\n${OU}\n${CN}\n${email}\n\n\n" | \
openssl req \
    -config openssl-ocsp-revoked.conf -new -key $fio-$group-ocsp-revoked.key -out \
    $fio-$group-ocsp-revoked.crt -passin pass:

openssl x509 -req -days 90 \
    -extensions v3_ca -extfile openssl-ocsp-revoked.conf \
    -CA $fio-$group-intr.crt \
    -CAkey $fio-$group-intr.key \
    -CAcreateserial \
    -CAserial serial \
    -in $fio-$group-ocsp-revoked.crt -out $fio-$group-ocsp-revoked.crt \
    -passin pass:$fio

# Отзываем сертификат
openssl ca -config openssl-intr.conf \
           -keyfile shushuevai-msp21-intr.key \
           -cert shushuevai-msp21-intr.crt \
           -revoke shushuevai-msp21-ocsp-revoked.crt \
           -passin pass:shushuevai

# Собираем архив
zip $fio-$group-p1_3.zip $fio-$group-ocsp-valid.*  $fio-$group-ocsp-revoked.* \
    $fio-$group-ocsp-resp.* $fio-$group-chain.crt

# Поднимаем ocsp сервер
# openssl ocsp -port 2560 -index cadir/index.txt -CA shushuevai-msp21-chain.crt -rkey shushuevai-msp21-ocsp-resp.key -rsigner shushuevai-msp21-ocsp-resp.crt -text -out ocsp.log
