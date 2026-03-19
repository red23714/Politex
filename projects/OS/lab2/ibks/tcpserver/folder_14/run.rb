require './base.rb'
# timelim(5)
# clients(5)

testing do 
  srv = server(TCPSRVBIN, "9114")

  cli = []
  1.upto(100) do |i|
    cli[i] = client(TCPCLIEMUL, "127.0.0.1:9114", "cli1.txt", "--nosend=1")
  end

  log "Clients waiting..."
  1.upto(100) do |i|
    cli[i].wait
  end

  cli = client(TCPCLIEMUL, "127.0.0.1:9114", "cli2.txt")
  log "Client1 waiting..."
  cli.wait
  
  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
