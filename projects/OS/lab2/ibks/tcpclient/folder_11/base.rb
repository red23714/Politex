require 'rubygems'
require 'fileutils'
require 'childprocess'

$pwd = Dir.pwd

TCPSRVEMUL = "tcpserveremul.rb"
TCPCLIEMUL = "tcpclientemul.rb"

TCPCLIBIN = "tcpclient"
TCPSRVBIN = "tcpserver"
#TCPCLIBIN = "tcpclientemul.rb"
#TCPSRVBIN = "tcpserveremul.rb"


def log(msg)
  File.open("run.log", "at") {|f| f.puts(msg)}
end

def process(stdout_fn, bin, *ext_args)
  bin = "#{bin}.exe" if File.exist?("#{bin}.exe")
  args = ["#{$pwd}/#{bin}"] + ext_args
  if bin.index(".rb")
    args = ["ruby"] + args
  elsif bin.index(".exe") && Module.constants.include?(:WINEBIN)
    args = [WINEBIN] + args 
  end

  log "Starting: #{args.join(' ')}"
  prc = ChildProcess.build(*args)
  prc.cwd = "#{$pwd}"
  prc.environment.merge!(WINEENV) if bin.index(".exe") && Module.constants.include?(:WINEENV)

  f = File.open(stdout_fn, "w")
  f.sync = true
  prc.io.stdout = prc.io.stderr = f

  prc.start
  prc  
end

def server(bin, *ext_args)
  srv = process("srv.stdout", bin, *ext_args)
  sleep(0.2)
  srv    
end

$clinum = 0
def client(bin, *ext_args)
  $clinum = $clinum + 1
  process("cli%d.stdout" % $clinum, bin, *ext_args)
end


def testing(options = {}, &block)
  begin
    block.call
  rescue Exception => e
    log "Exception: #{e.inspect}\n#{e.backtrace.shift(5).join("\n")}"
  ensure    
  end  

  {"win_profiler.txt" => "mem.txt", "lin_profiler.txt" => "mem.txt" }.each do |src,dst|
    begin
      if File.exist?("#{$pwd}/#{src}")
        FileUtils.cp("#{$pwd}/#{src}", "#{$pwd}/#{dst}") 
        log "File #{dst} copied."
      end
    rescue Exception => e
      log "Error on copy #{dst}: #{e.inspect}"
    end
  end
end

