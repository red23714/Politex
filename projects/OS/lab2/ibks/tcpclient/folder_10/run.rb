require './base.rb'
# timelim(4)
# clients(2)

testing do 
  srv = server(TCPSRVEMUL, "9010", "--cndrop=300")

  cli = client(TCPCLIBIN, "127.0.0.1:9010", "cli1.txt")
  log "Client1 waiting..."
  cli.wait

  cli = client(TCPCLIBIN, "127.0.0.1:9010", "cli2.txt")
  log "Client2 waiting..."
  cli.wait
  
  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
