package main

import (
	"bufio"
	"fmt"
	"log"
	"os/exec"

	"github.com/streadway/amqp"
)

func failOnError(err error, msg string) {
	if err != nil {
		log.Fatalf("%s: %s", msg, err)
	}
}

func main() {
	conn, err := amqp.Dial("amqp://guest:guest@<server ip>:5672/")
	failOnError(err, "Failed to connect to RabbitMQ")
	defer conn.Close()

	ch, err := conn.Channel()
	failOnError(err, "Failed to open a channel")
	defer ch.Close()
	//args := make(amqp.Table)
	//args["max-length"] = int32(1)
	q, err := ch.QueueDeclare(
		"logger", // name
		false,    // durable
		false,    // delete when unused
		false,    // exclusive
		false,    // no-wait
		nil,      // arguments
	)
	failOnError(err, "Failed to declare a queue")
	fmt.Println(q)
	i := 0
	cmd := exec.Command("sudo", "tcpdump", "-i", "any")
	stdout, err := cmd.StdoutPipe()
	cmd.Start()
	rd := bufio.NewReader(stdout)
	for {
		str, err := rd.ReadString('\n')
		if err != nil {
			log.Fatal("Read Err:", err)
			return
		}
		log.Printf("%d [x] Sent %s", i, str)
		err = ch.Publish(
			"",     // exchange
			q.Name, // routing key
			false,  // mandatory
			false,  // immediate
			amqp.Publishing{
				ContentType: "text/plain",
				Body:        []byte(str),
			})
		i++
		if err != nil {
			log.Fatal("Read Err:", err)
			return
		}
	}
}
