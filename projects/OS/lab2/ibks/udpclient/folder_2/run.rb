require './base.rb'
# timelim(3) 
# clients(2)

testing do 
  srv = server(UDPSRVEMUL, "9020", "9020")

  cli = client(UDPCLIBIN, "127.0.0.1:9020", "cli1.txt")
  log "Client-1 waiting..."
  cli.wait

  cli = client(UDPCLIBIN, "127.0.0.1:9020", "cli2.txt")
  log "Client-2 waiting..."
  cli.wait  

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
