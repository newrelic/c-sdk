# Agent Integration Testing:  redis

A collection of procedures to exercise redis.

## Redis Installation

NOTE: as of October 2014, the version of redis in the CentOS 6.x repository is <2.6.
This version is not modern enough to pass all of our integration/redis AIT tests.

### Via Docker

To install redis as a docker container in the [nrlamp-centos65-x64 VM](https://source.datanerd.us/php-agent/php_test_tools/tree/master/scripts/nrlamp-centos65-x64), you can use the following recipe:

    sudo yum -y install docker-io
    sudo service docker start
    sudo chkconfig docker on
    sudo docker run -d --name redis -p 6379:6379 dockerfile/redis

### Via Yum

To install a more current redis in the [nrlamp-centos65-x64 VM](https://source.datanerd.us/php-agent/php_test_tools/tree/master/scripts/nrlamp-centos65-x64), you can use the following recipe (inspired by [the redis quickstart guide](http://redis.io/topics/quickstart)):

    sudo yum -y install tcl
    
    wget http://download.redis.io/redis-stable.tar.gz
    tar xvzf redis-stable.tar.gz
    cd redis-stable
    sudo mv /opt/nr/ /opt/nr.stash
    make
    make test
    sudo make install
    sudo mv /opt/nr.stash/ /opt/nr

    redis-server 1> /dev/null &

NOTE: the temporary rename of /opt/nr is to provide an easy way to get NRCAMP out of the path
while building redis.


## Test Configuration

Connection can be configured via environment variables:

  * REDIS_HOST (default: "127.0.0.1")
  * REDIS_PORT (default: 6379)

## Notes
    TODO: psetex
    TODO: incrByFloat
    TODO(rrh): Many other functions mentioned in the manual, but don't appear here
    See:
      https://github.com/nicolasff/phpredis
    
    redis::connect    =>  "Memcache/connect"
    redis::del        =>  "Memcache/delete"
    redis::delete     =>  "Memcache/delete"
    redis::get        =>  "Memcache/get"
    redis::set        =>  "Memcache/set"
    redis::setex      =>  "Memcache/set"
    redis::setnx      =>  "Memcache/set"
    redis::incr       =>  "Memcache/incr"
    redis::incrby     =>  "Memcache/incr"
    redis::decr       =>  "Memcache/decr"
    redis::decrby     =>  "Memcache/decr"
    redis::lset       =>  "Memcache/set"
    redis::lget       =>  "Memcache/get"
    redis::lrem       =>  "Memcache/delete"
    redis::lremove    =>  "Memcache/delete"
    redis::lindex     =>  "Memcache/get"
    redis::hset       =>  "Memcache/set"
    redis::hsetnx     =>  "Memcache/set"
    redis::hget       =>  "Memcache/get"
    redis::hmset      =>  "Memcache/set"
    redis::hmget      =>  "Memcache/get"