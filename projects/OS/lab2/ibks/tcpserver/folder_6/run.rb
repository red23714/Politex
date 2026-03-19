require './base.rb'
# timelim(8)
# clients(1)

testing do 
  srv = server(TCPSRVBIN, "9106")
  cli = client(TCPCLIEMUL, "127.0.0.1:9106", "cli1.txt")

  log "Client waiting..."
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
