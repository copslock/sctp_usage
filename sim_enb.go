package main

import (
	"flag"
	"log"
	"net"
	"strings"
	"github.com/ishidawataru/sctp"
)

func serveClient(conn net.Conn) error {
	for {
		buf := make([]byte, 254)
		n, err := conn.Read(buf)
		if err != nil {
			return err
		}
		n, err = conn.Write(buf[:n])
		if err != nil {
			return err
		}
		log.Printf("write: %d", n)
	}
}

func main() {
	var ip = flag.String("ip", "0.0.0.0", "")
	attachreq := []byte{0x00, 0x0c, 0x40, 0x3f, 0x00, 0x00, 0x05, 0x00, 0x08, 0x00, 0x02, 0x00, 0x07, 0x00, 0x1a,
		0x00, 0x17, 0x16, 0x07, 0x41, 0x71, 0x08, 0x99, 0x89, 0x89, 0x00, 0x00, 0x00, 0x01, 0x10, 0x02, 0x80, 0xc0,
		0x00, 0x05, 0x02, 0x00, 0xd0, 0x11, 0xd1, 0x00, 0x43, 0x00, 0x06, 0x00, 0x99, 0xf8, 0x89, 0x00, 0x01, 0x00,
		0x64, 0x40, 0x08, 0x00, 0x99, 0xf8, 0x89, 0x00, 0x01, 0x1d, 0x70, 0x00, 0x86, 0x40, 0x01, 0x30}

	var port = flag.Int("port", 0, "")
	flag.Parse()

	ips := []net.IP{}

	for _, i := range strings.Split(*ip, ",") {
		ips = append(ips, net.ParseIP(i))
	}

	addr := &sctp.SCTPAddr{
		IP:   ips,
		Port: *port,
	}

	conn, err := sctp.DialSCTP("sctp", nil, addr)
	if err != nil {
		log.Fatalf("failed to dial: %v", err)
	}
	ppid := 0
	info := &sctp.SndRcvInfo{
		Stream: uint16(ppid),
		PPID:   uint32(0x12000000),
		}
	conn.SubscribeEvents(sctp.SCTP_EVENT_DATA_IO)
	n, err := conn.SCTPWrite(attachreq, info)
	if err != nil {
		log.Fatalf("failed to write: %v", err)
	}
	log.Printf("write: %d", n)
}
