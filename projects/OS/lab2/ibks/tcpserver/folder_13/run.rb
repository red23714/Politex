require './base.rb'
# timelim(3)
# clients(2)

testing do 
  srv = server(TCPSRVBIN, "9113")

  cli = client(TCPCLIEMUL, "127.0.0.1:9113", "cli1.txt", "--nosend=1")
  log "Client0 waiting..."
  cli.wait

  cli = client(TCPCLIEMUL, "127.0.0.1:9113", "cli2.txt")
  log "Client1 waiting..."
  cli.wait
  
  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
