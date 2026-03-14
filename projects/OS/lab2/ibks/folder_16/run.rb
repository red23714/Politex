require './base.rb'
# timelim(8) 
# clients(41)

testing do 
  srv = server(UDPSRVBIN, "9950", "9957")

  log "Starting clients..."
  cli = []
  1.upto(40) do |i| 
    cli[i] = client(UDPCLIEMUL, "127.0.0.1:9950", "cli%d.txt" % i)
  end

  log "Waiting clients..."
  1.upto(40) { |i| cli[i].wait }

  log "Stopping..."
  cli = client(UDPCLIEMUL, "127.0.0.1:9950", "cli41.txt")
  cli.wait  

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
