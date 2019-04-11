FROM php:7.0-cli

RUN apt-get update
RUN apt-get install -y zip unzip curl git
RUN php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"
RUN php composer-setup.php --install-dir=/usr/bin --filename=composer
RUN php -r "unlink('composer-setup.php');"
RUN mkdir /opt/stress-test-validate-results
ADD src /opt/stress-test-validate-results/src
ADD composer.json /opt/stress-test-validate-results/composer.json
ADD test.php /opt/stress-test-validate-results/test.php
RUN cd /opt/stress-test-validate-results/; composer install

ENTRYPOINT ["php", "/opt/stress-test-validate-results/test.php"]
