require './base.rb'
# timelim(8) 
# clients(41)

testing do 
  srv = server(UDPSRVBIN, "9970", "9979")
  myip = "10.0.170.90"

  log "Starting clients..."

  ips = ["127.0.0.1", myip]
  ports = 9970.upto(9979).collect {|p| p}

  cli = []
  1.upto(40) do |i| 
    ip = ips[i % ips.size]
    port = ports[i % ports.size]                                          
    cli[i] = client(UDPCLIEMUL, "#{ip}:#{port}", "cli%d.txt" % i, "--sendrep=4")
  end

  log "Waiting clients..."
  1.upto(40) { |i| cli[i].wait }

  log "Stopping..."
  cli = client(UDPCLIEMUL, "127.0.0.1:9970", "cli41.txt")
  cli.wait  

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
