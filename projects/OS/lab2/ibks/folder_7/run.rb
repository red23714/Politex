require './base.rb'
# timelim(5)
# clients(51)

testing do
  cnt = 50
  srv = server(TCPSRVEMUL, "9007")
  cli = []
  1.upto(cnt) { |i| cli[i] = client(TCPCLIBIN, "127.0.0.1:9007", "cli%d.txt" % i) }
  
  log "Waiting for clients..."
  1.upto(50) {|i| cli[i].wait }
  
  log "Sending 'stop'..."
  cli = client(TCPCLIBIN, "127.0.0.1:9007", "cli%d.txt" % (cnt+1) )
  cli.wait

  log "Server waiting..."
  srv.wait
  log "Server exit code: #{srv.exit_code}"
end
