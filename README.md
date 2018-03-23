This is a blanket repo for all the test apps created for testing LTE/serverless usecases.

export GOPATH=$PWD
go get github.com/ishidawataru/sctp

Simulated SGW:
go run http_server.go

MME Proxy:
go run http_client.go <ue_resp_ip> <sgw_req_ip>

Simulated enb:
go run sim_enb.go -ip=<interface ip> -port=36412

Simulated enb written in C:
cd c_http_enb
gcc sctp_server.c -l sctp -l curl -o sctp_server
This works in two modes:
1. Run it as a proxy between serverless function and enodeb. This
sends http request for each sctp message and replies with the result
returned from the functions.
./sctp_server -f

2. To run it in local mode where we can use the MCP as standalone
application (exactly same as what is present in serverless function),
provide the path to the executable of MCP using -p parameter, eg.

./sctp_server -p $HOME/MobileControlPlanes/build/mymme/src/mymme
