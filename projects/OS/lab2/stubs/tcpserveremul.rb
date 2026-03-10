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


def msglog(peer, items)
  msg = "#{peer} #{items.collect {|item| item.text}.join(" ")}"
  File.open("msg.txt", "a"){|f| f.puts(msg) }
end

def shift_msg(buf, peer)
  data = buf.dup
  begin
    raise InvalidData.new if data.size < 4
    data.slice!(0,4)
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


def eat_space(str)
  raise "Invalid string: #{str}" if str.size == 0 || str[0..0] != " "
  str.slice!(0, 1)
end


def send_msg(cn, idx, str)
  sendbuf = "" 
  sendbuf << [idx].pack("N")

  $dsp.each do |d|
    sendbuf << d.new.proto(str)
    eat_space(str) if d != $dsp.last
  end
  
  cn.write(sendbuf)
end


def send_msgs(cn)
  File.open("msg.txt", "r") do |f|
    r = Regexp.new("^[\\d\\.\\:]+\\s(.*?)$")
    idx = 0
    while (str = f.gets) != nil
      str = str.chomp
      m = r.match(str)
      if m && !str.empty?
        send_msg(cn, idx, "#{m[1]}")
        idx = idx + 1
      end
    end
    puts "#{idx} messages sent."
  end
end


def dispatch(cn, peer)
  buf = ""
  mode = nil
  loop do
    data = cn.recv(512)
    break if data.size == 0
    buf << data

    unless mode
      if buf.size >= 3
        mode = buf.slice!(0,3)
        unless ["put", "get"].include?(mode)
          raise "Invalid client cmd: #{mode}"
        end
      end
    end

    if mode == "put"
      while text = shift_msg(buf, peer)
        cn.write("ok")

        if text == "stop"
          puts "'stop' message arrived. Terminating..."
          Kernel.exit 
        end
      end
    elsif mode == "get"
      send_msgs(cn)
      return
    end    
  end
end

def server(port)
  server = TCPServer.new("0.0.0.0", port)
  puts "Listening TCP port: #{port}"
  loop do
    Thread.fork(server.accept) do |cn|
      peer = "#{cn.peeraddr[3]}:#{cn.peeraddr[1]}"
      puts "  Peer connected  : #{peer}"
      begin
        dispatch(cn, peer)
      rescue Exception => e
        raise e if e.is_a?(SystemExit)
        puts "  Peer Exception   : #{peer}: '#{e.message}'"
        puts "#{e.backtrace.shift(3).join("\n")}"
      ensure
        puts "  Peer disconnected: #{peer}"
        cn.close
      end
    end
  end
end

server(ARGV[0] || 9000)
