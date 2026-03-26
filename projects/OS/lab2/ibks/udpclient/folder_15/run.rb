require './base.rb'
# timelim(4) 
# clients(11)

testing do 
  srv = server(UDPSRVEMUL, "9150", "9150")

  log "Starting clients..."
  cli = []
  1.upto(10) do |i| 
    ip = (i >= 3 && i <= 6) ? "127.0.0.1" : "127.0.0.1" # fix ip
    cli[i] = client(UDPCLIBIN, "#{ip}:9150", "cli%d.txt" % i)
  end

  log "Waiting clients..."
  1.upto(10) {|i| cli[i].wait}

  log "Stopping server..."
  cli = client(UDPCLIBIN, "127.0.0.1:9150", "cli11.txt")
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
