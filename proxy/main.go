package main

import (
    "bytes"
    "encoding/json"
    "io"
    "net/http"
    "os"
    "time"
    "log"
)

var targets []string
var client = &http.Client{Timeout: 5 * time.Second}

func handler(w http.ResponseWriter, r *http.Request) {
	body, err := io.ReadAll(r.Body)
	if err != nil {
		http.Error(w, "read error", http.StatusBadRequest)
		return
	}
	log.Printf("-> %s %s (%d bytes)", r.Method, r.URL.RequestURI(), len(body))
	for _, target := range targets {
		req, _ := http.NewRequest("POST", target+r.URL.RequestURI(), bytes.NewReader(body))
		req.Header.Set("Content-Type", r.Header.Get("Content-Type"))
		go func(req *http.Request, target string) {
			resp, err := client.Do(req)
			if err != nil {
				log.Printf("   %s error: %v", target, err)
				return
			}
			log.Printf("   %s %d", target, resp.StatusCode)
		}(req, target)
	}
	w.WriteHeader(http.StatusNoContent)
}

func main() {
	if len(os.Args) < 2 {
		log.Fatal("usage: forwarder <config.json>")
	}
	data, err := os.ReadFile(os.Args[1])
	if err != nil {
		panic(err)
	}
	if err := json.Unmarshal(data, &targets); err != nil {
		panic(err)
	}

	http.HandleFunc("/", handler)
	http.ListenAndServe(":9000", nil)
}
