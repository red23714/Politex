require './base.rb'
# timelim(3) 
# clients(2)

testing do 
  srv = server(UDPSRVBIN, "9920", "9925")

  cli = client(UDPCLIEMUL, "127.0.0.1:9921", "cli1.txt")
  log "Client-1 waiting..."
  cli.wait

  cli = client(UDPCLIEMUL, "127.0.0.1:9923", "cli2.txt")
  log "Client-2 waiting..."
  cli.wait  

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
