This is a blanket repo for all the test apps created for testing LTE/serverless usecases.

export GOPATH=$PWD
go get github.com/ishidawataru/sctp

Simulated SGW:
go run http_server.go

MME Proxy:
go run http_client.go <ue_resp_ip> <sgw_req_ip>

Simulated enb:
go run sim_enb.go -ip=<interface ip> -port=36412
