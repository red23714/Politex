require './base.rb'
# timelim(8)
# clients(1)

testing do 
  srv = server(TCPSRVEMUL, "9006")
  cli = client(TCPCLIBIN, "127.0.0.1:9006", "cli1.txt")

  log "Client waiting..."
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
