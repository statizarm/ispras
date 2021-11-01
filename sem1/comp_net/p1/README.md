# Конфиги и скрипт нужно отредактировать!!!

# P3

## Поднимаем ocsp сервер
`openssl ocsp -port 2560 -index cadir/index.txt -CA shushuevai-msp21-chain.crt -rkey shushuevai-msp21-ocsp-resp.key -rsigner shushuevai-msp21-ocsp-resp.crt -text -out ocsp.log`

## Собираем контейнер с nginx'ом
`docker build -t my_nginx .`

## Запускаем
(костыльненько)
`docker run --name my_nginx -it -p 443:443 my_nginx /bin/bash`

(в виртуальной консоли)
`nginx`

