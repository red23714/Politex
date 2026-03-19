require './base.rb'
# timelim(6)
# clients(61)

testing do 
  srv = server(TCPSRVBIN, "9109")
  cnt = 60

  cli = []
  1.upto(cnt) do |i| 
    cli[i] = client(TCPCLIEMUL, "127.0.0.1:9109", "cli%d.txt" % i)
  end

  log "Clients waiting..."
  1.upto(cnt) { |i| cli[i].wait }

  log "Stopping server..."
  cli = client(TCPCLIEMUL, "127.0.0.1:9109", "cli%d.txt" % (cnt+1))
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
