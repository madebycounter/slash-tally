run:
	go run main.go

build-mac:
	GOOS=darwin GOARCH=arm64 CGO_ENABLED=0 go build -o forwarder main.go

build-router:
	GOOS=linux GOARCH=arm64 CGO_ENABLED=0 go build -o forwarder main.go