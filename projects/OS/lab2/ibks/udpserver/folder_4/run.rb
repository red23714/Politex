require './base.rb'
# timelim(6) 
# clients(11)

testing do 
  srv = server(UDPSRVBIN, "9940", "9945")

  log "Starting clients..."
  cli = []
  1.upto(10) do |i| 
    cli[i] = client(UDPCLIEMUL, "127.0.0.1:9941", "cli%d.txt" % i)
    Kernel.sleep(0.08)
  end

  log "Waiting clients..."
  1.upto(10) { |i| cli[i].wait }

  log "Stopping..."
  cli = client(UDPCLIEMUL, "127.0.0.1:9940", "cli11.txt")
  cli.wait  

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
