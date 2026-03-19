require './base.rb'
# timelim(5)
# clients(4)

testing do 
  srv = server(TCPSRVBIN, "9116")

  cli = []
  1.upto(3) do |i|
    cli[i] = client(TCPCLIEMUL, "127.0.0.1:9116", "cli%d.txt" % i, "--cndrop=#{100 + i*5}")
  end

  log "Clients waiting..."
  1.upto(3) { |i| cli[i].wait }

  cli = client(TCPCLIEMUL, "127.0.0.1:9116", "cli4.txt")
  log "Client waiting..."
  cli.wait
  
  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
