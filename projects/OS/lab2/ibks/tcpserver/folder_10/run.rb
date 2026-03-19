require './base.rb'
# timelim(4)
# clients(1)

testing do 
  srv = server(TCPSRVBIN, "9110")

  cli = client(TCPCLIEMUL, "127.0.0.1:9110", "cli1.txt", "--slowput=0.5")
  log "Client waiting..."
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
