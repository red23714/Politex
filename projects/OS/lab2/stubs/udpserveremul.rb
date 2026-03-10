require 'socket.rb'

class InvalidData < Exception
end



class DspPdate
  def load(data)
    if data.size >= 4
      val = data.slice!(0, 4).unpack("N")[0]
      @day = val % 100
      @month = val / 100 % 100
      @year = val / 10000
    else
      raise InvalidData.new
    end
  end
  
  def text
    "%02d.%02d.%04d" % [@day, @month, @year]
  end

  def proto(str)
    r = Regexp.new("^(\\d{2})\\.(\\d{2})\\.(\\d{4})")
    m = r.match(str)
    raise InvalidData.new unless m
    str.slice!(0, m[0].size)
    
    [ m[3].to_i * 10000 + m[2].to_i * 100 + m[1].to_i ].pack("N")
  end
end


class DspPtime
  def load(data)
    if data.size >= 4
      val = data.slice!(0, 4).unpack("N")[0]
      @ss = val % 100
      @mm = val / 100 % 100
      @hh = val / 10000      
    else
      raise InvalidData.new
    end
  end
  
  def text
    "%02d:%02d:%02d" % [@hh, @mm, @ss]
  end

  def proto(str)
    r = Regexp.new("^(\\d{2})\\:(\\d{2})\\:(\\d{2})")
    m = r.match(str)
    raise InvalidData.new unless m
    str.slice!(0, m[0].size)
    [m[1].to_i * 10000 + m[2].to_i * 100 + m[3].to_i].pack("N")
  end
end



class DspMsgbt
  def load(data)
    if data.size >= 4
      val = data.slice!(0,4).unpack("N")[0]
      if data.size >= val
        @val = data.slice!(0, val)
      else
        raise InvalidData.new
      end
    else
      raise InvalidData.new
    end    
  end
  
  def text
    @val
  end

  def proto(str)  
    buf = [str.size].pack("N")
    buf << str
    str.replace("")
    buf
  end
end

$dsp = [DspPdate, DspPtime, DspPtime, DspMsgbt]


$db = {}

def msglog(peer, items)
  msg = "#{peer} #{items.collect {|item| item.text}.join(" ")}"
  File.open("msg.txt", "a"){|f| f.puts(msg) }
end


def shift_msg(buf, peer)
  data = buf.dup
  begin
    items = $dsp.collect do |d| 
      item = d.new
      item.load(data)
      item
    end
    buf.replace(data)
    msglog(peer, items)
    return items.last.text
  rescue InvalidData => e
    return nil
  end
  nil
end


def process_recv(sock)
  text = nil
  dg, cli = sock.recvfrom_nonblock(65535)

  peer = "#{cli[3]}:#{cli[1]}"
  unless $db[peer]
    puts "New client detected: #{peer}"
    $db[peer] = {:last_tm => Time.now.to_i, :rcv => []}
  end

  begin
    idx = dg.slice!(0,4).unpack("N")[0]
    
    unless $db[peer][:rcv].include?(idx) #prevent duplicates
      text = shift_msg(dg, peer) # Parse and store message
      if text
        $db[peer][:rcv].push(idx)
      else
        puts "error: Invalid datagram received from #{peer}"
      end
    end
    $db[peer][:last_tm] = Time.now.to_i
  rescue Exception => e
    puts "Error in datagram from peer: #{peer}: #{e.message}\n#{e.backtrace.shift(3).join("\n")}"
  end

  # Send response
  if $db[peer][:rcv].size > 0
    received = $db[peer][:rcv]
    cnt = 0
    sendbuf = ""
    (received.size-1).downto(0).each do |i|
      sendbuf << [received[i]].pack("N")
      cnt = cnt + 1
      break if cnt.size >= 20
    end
    p = peer.split(":")
    sock.send(sendbuf, 0, p.first, p.last.to_i)
  end

  if text == "stop"
    puts "'stop' message arrived. Terminating..."
    Kernel.exit 
  end

end


def cleanup_old
  ct = Time.now.to_i

  old = $db.collect {|peer,info| (info[:last_tm] + 30 < ct) ? peer : nil}.compact
  old.each do |peer| 
    $db.delete(peer)
    puts "Client removed from db: #{peer}"
  end
end


def server(port_from, port_till)
  ports = port_from.upto(port_till).collect {|i| i}
  raise "No port to listen" if ports.size == 0
  puts "Listening on: #{ports.join(", ")}"

  sockets = ports.collect do |p|
    socket = UDPSocket.new(Socket::AF_INET)
    socket.bind("0.0.0.0", p)
    socket
  end

  loop do
    ready = IO.select(sockets, nil, nil, 30)
    ready[0].each { |sock| process_recv(sock) } if ready
    cleanup_old    
  end

end


server(ARGV[0] || 9000, ARGV[1] || (ARGV[0] || 9000))
