require './base.rb'
# timelim(4)
# clients(2)

testing do 
  srv = server(TCPSRVBIN, "9115")

  cli = client(TCPCLIEMUL, "127.0.0.1:9115", "cli1.txt", "--nowait=1")
  log "Client1 waiting..."
  cli.wait

  Kernel.sleep(1)

  cli = client(TCPCLIEMUL, "127.0.0.1:9115", "cli2.txt")
  log "Client2 waiting..."
  cli.wait
  
  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
