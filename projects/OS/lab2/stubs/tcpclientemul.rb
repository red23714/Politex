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
  s.write(sendbuf)
end

def wait_confirm(s)
  buf = s.recv(2)
  puts "recv: #{buf.unpack("C*").collect {|b| "%02x" % b} }" if ARGV.include?("--dump")
  raise "Invalid confirmation" if buf != "ok"
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
  s.write("put")

  r = Regexp.new("^(.*?)$")
  File.open(filename, "r") do |f|
    idx = 0
    while (str = f.gets) != nil
      str = str.chomp
      m = r.match(str)
      if m && !str.empty?
        upload_msg(s, idx, "#{m[1]}")
        wait_confirm(s)
        idx = idx + 1
      end
    end    

    puts "#{idx} message(s) has been sent."
  end

  s.close
end

raise "Invalid args" if ARGV.size < 2
client(ARGV[0], ARGV[1])
