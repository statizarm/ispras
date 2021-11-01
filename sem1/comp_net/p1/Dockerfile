FROM nginx:latest

COPY ./shushuevai-msp21-ocsp-valid.* /var/www/ocsp.valid.shushuevai.ru/
COPY ./shushuevai-msp21-ocsp-revoked.* /var/www/ocsp.revoked.shushuevai.ru/

COPY ./sites/html/valid.html /var/www/ocsp.valid.shushuevai.ru/index.html
COPY ./sites/html/revoked.html /var/www/ocsp.revoked.shushuevai.ru/index.html

COPY ./sites/conf/nginx.conf /etc/nginx/nginx.conf

