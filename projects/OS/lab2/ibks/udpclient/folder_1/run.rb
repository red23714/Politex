require './base.rb'
# timelim(2) 
# clients(1)

testing do 
  srv = server(UDPSRVEMUL, "9010", "9010")
  cli = client(UDPCLIBIN, "127.0.0.1:9010", "cli1.txt")

  log "Client waiting..."
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
