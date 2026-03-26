require './base.rb'
# timelim(4) 
# clients(11)

testing do 
  srv = server(UDPSRVEMUL, "9030", "9030")

  log "Starting clients..."
  cli = []
  1.upto(10) {|i| cli[i] = client(UDPCLIBIN, "127.0.0.1:9030", "cli%d.txt" % i) }

  log "Waiting clients..."
  1.upto(10) {|i| cli[i].wait}

  log "Stopping server..."
  cli = client(UDPCLIBIN, "127.0.0.1:9030", "cli11.txt")
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
