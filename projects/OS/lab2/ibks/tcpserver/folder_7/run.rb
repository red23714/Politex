require './base.rb'
# timelim(5)
# clients(31)

testing do 
  srv = server(TCPSRVBIN, "9107")
  cnt = 30

  cli = []
  1.upto(cnt) { |i| cli[i] = client(TCPCLIEMUL, "127.0.0.1:9107", "cli%d.txt" % i) }

  log "Clients waiting..."
  1.upto(cnt) { |i| cli[i].wait }

  log "Stopping server..."
  cli = client(TCPCLIEMUL, "127.0.0.1:9107", "cli%d.txt" % (cnt+1))
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
