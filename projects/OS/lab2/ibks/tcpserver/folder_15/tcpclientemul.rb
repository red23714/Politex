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



  $slowput = nil
  $slowsend = nil
  $nosend = nil
  $nowait = nil
  $cndrop = nil

  $cntotal = 0

def eat_space(str)
  raise "Invalid string: #{str}" if str.size == 0 || str[0..0] != " "
  str.slice!(0, 1)
end

def upload_msg(s, idx, str)
  sendbuf = ""
  sendbuf << [idx].pack("N")
  $dsp.each do |d|
    sendbuf << d.new.proto(str)
    eat_space(str) if d != $dsp.last
  end

  # Debug log: dump protocol
  puts "send: #{sendbuf.unpack("C*").collect {|b| "%02x" % b} }" if ARGV.include?("--dump")

  if $slowsend == nil && $cndrop == nil
    s.write(sendbuf)
    $cntotal = $cntotal + sendbuf.size
  else
    0.upto(sendbuf.size-1) do |i|
      part = sendbuf[i..i]
      s.write(part)
      $cntotal = $cntotal + part.size      
      Kernel.sleep($slowsend) if $slowsend
      raise "Disconnected: cndrop=#{$cndrop}." if $cndrop && ($cntotal > $cndrop)
    end
  end
end

def wait_confirm(s)
  resp = ""
  while resp.size < 2
    buf = s.recv(2 - resp.size)
    raise "Disconnected by server." if buf.size == 0
    resp << buf
  end
  puts "recv: #{resp.unpack("C*").collect {|b| "%02x" % b} }" if ARGV.include?("--dump")
  raise "Invalid confirmation" if resp != "ok"
end

def client(serv_addr, filename)
  s = nil

  puts "Connecting to: #{serv_addr}"

  p = serv_addr.split(":")
  10.times do
    begin
      s = TCPSocket.new(p[0], p[1].to_i)
      break
    rescue Exception => e
      puts e.message
      Kernel.sleep(0.1)
    end  
  end
  raise "Failed connect" unless s

  puts "Connected."
  
  ARGV.each do |ar|
    m = /^\-\-slowput=(.*?)$/.match(ar)
    $slowput = "#{m[1]}".to_f if m

    m = /^\-\-slowsend=(.*?)$/.match(ar)
    $slowsend = "#{m[1]}".to_f if m

    m = /^\-\-nosend=(.*?)$/.match(ar)
    $nosend = "#{m[1]}".to_i if m

    m = /^\-\-nowait=(.*?)$/.match(ar)
    $nowait = "#{m[1]}".to_i if m

    m = /^\-\-cndrop=(.*?)$/.match(ar)
    $cndrop = "#{m[1]}".to_i if m
  end

  unless $slowput
    s.write("put")
  else
    s.write("p")
    Kernel.sleep($slowput)
    s.write("u")
    Kernel.sleep($slowput)
    s.write("t")
  end

  if $nosend && $nosend == 1
    puts "nosend=1. Client terminated."
    return
  end

  r = Regexp.new("^(.*?)$")
  File.open(filename, "r") do |f|
    idx = 0
    while (str = f.gets) != nil
      str = str.chomp
      m = r.match(str)
      if m && !str.empty?
        upload_msg(s, idx, "#{m[1]}")
        wait_confirm(s) if $nowait == nil || $nowait == 0
        idx = idx + 1
      end
    end    

    puts "#{idx} message(s) has been sent."
  end

  s.close
end

raise "Invalid args" if ARGV.size < 2
client(ARGV[0], ARGV[1])
