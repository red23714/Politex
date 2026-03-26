require './base.rb'
# timelim(3)
# clients(1)

testing do 
  srv = server(UDPSRVEMUL, "9070", "9070", "--skipfirst=10")

  cli = client(UDPCLIBIN, "127.0.0.1:9070", "cli1.txt")
  log "Client waiting..."
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
