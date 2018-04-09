package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"
	"os"

	"github.com/streadway/amqp"
)

func failOnError(err error, msg string) {
	if err != nil {
		log.Fatalf("%s: %s", msg, err)
	}
}

func main() {
	forever := make(chan bool)
	dial_str := fmt.Sprintf("amqp://guest:guest@<server ip>:5672/")
	conn, err := amqp.Dial(dial_str)
	failOnError(err, "Failed to connect to RabbitMQ")
	defer conn.Close()

	ch, err := conn.Channel()
	failOnError(err, "Failed to open a channel")
	defer ch.Close()
	q, err := ch.QueueDeclare(
		"logger", // name
		false,    // durable
		false,    // delete when unused
		false,    // exclusive
		false,    // no-wait
		nil,      // arguments
	)
	failOnError(err, "Failed to declare a queue")

	msgs, err := ch.Consume(
		q.Name, // queue
		"",     // consumer
		true,   // auto-ack
		false,  // exclusive
		false,  // no-local
		false,  // no-wait
		nil,    // args
	)
	failOnError(err, "Failed to register a consumer")
	file, err := os.OpenFile("logs/log1.txt", os.O_RDWR|os.O_CREATE, 0755)
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
	defer file.Close()
	go func(file *os.File) {
		for d := range msgs {
			log.Printf("Received a message: %s", d.Body)
			n, err := file.Write([]byte(d.Body))
			if err != nil {
				fmt.Println(err)
				os.Exit(1)
			}
			if n != len(d.Body) {
				fmt.Println("failed to write data")
				os.Exit(1)
			}
		}
	}(file)
	log.Printf(" [*] Waiting for messages. To exit press CTRL+C")

	port := flag.String("p", "8100", "port to serve on")
	directory := flag.String("d", "./logs", "the directory of static file to host")
	flag.Parse()

	http.Handle("/", http.FileServer(http.Dir(*directory)))

	log.Printf("Serving %s on HTTP port: %s\n", *directory, *port)
	log.Fatal(http.ListenAndServe(":"+*port, nil))

	<-forever
}
