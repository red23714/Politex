require './base.rb'
# timelim(4)
# clients(3)

testing do 
  srv = server(TCPSRVEMUL, "9011")

  cli1 = client(TCPCLIBIN, "127.0.0.1:9011", "cli1.txt")
  cli2 = client(TCPCLIBIN, "10.0.170.90:9011", "cli2.txt")

  log "Clients 1,2 waiting..."
  cli1.wait
  cli2.wait

  cli3 = client(TCPCLIBIN, "127.0.0.1:9011", "cli3.txt")
  log "Client 3 waiting..."
  cli3.wait
  
  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
