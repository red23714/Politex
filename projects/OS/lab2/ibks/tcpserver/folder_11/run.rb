require './base.rb'
# timelim(6)
# clients(1)

testing do 
  srv = server(TCPSRVBIN, "9111")

  cli = client(TCPCLIEMUL, "127.0.0.1:9111", "cli1.txt", "--slowsend=0.015")
  log "Client waiting..."
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
