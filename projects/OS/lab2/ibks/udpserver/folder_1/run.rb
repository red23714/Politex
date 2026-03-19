require './base.rb'
# timelim(2) 
# clients(1)

testing do 
  srv = server(UDPSRVBIN, "9910", "9910")
  cli = client(UDPCLIEMUL, "127.0.0.1:9910", "cli1.txt")

  log "Client waiting..."
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
