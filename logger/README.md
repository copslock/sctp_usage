Tracer:

Tracer is a utility to stream logs from different servers to a
centralized location over rabbitmq.

Install rabbitmq on a server which collects logs:

    wget https://packages.erlang-solutions.com/erlang-solutions_1.0_all.deb
    
    sudo dpkg -i erlang-solutions_1.0_all.deb
    sudo apt-get update
    sudo apt-get install erlang
    sudo apt-get install rabbitmq-server
    sudo rabbitmq-plugins enable rabbitmq_management
    sudo service rabbitmq-server start
    echo "[{rabbit, [{loopback_users, []}]}]." >> /etc/rabbitmq/rabbitmq.config

Now you should be able to login to the management interface at:

    http://<server ip>:15672

Login with guest:guest

    sudo service rabbitmq-server restart

Install latest go:

    sudo curl -O https://storage.googleapis.com/golang/go1.9.1.linux-amd64.tar.gz
    sudo tar -xvf go1.9.1.linux-amd64.tar.gz
    sudo mv go /usr/local

    go get github.com/streadway/amqp
    go run receive.go

You can now open another browser tab to see the list of saved logs:
    
    http://<server ip>:8100

On the client machines run:

    go run send.go

Monitor:

Another set of scripts monitor.sh and start_capture.sh can be used for capturing
and displaying tcpdump on a remote server.

On the server under test, run:

    ./start_capture.sh <interface>

And on the monitor server run:

    ./monitor.sh interface_list.conf

