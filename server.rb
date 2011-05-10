require 'rubygems'
require 'eventmachine'
require 'json'

$pc = 1


class Connection
  attr_accessor :identifier, :player, :socket
  
  def initialize(obj_id, socket, player_id)
    @identifier = obj_id
    @player = player_id
    @socket = socket
  end
end



module EchoServer
  
  def send_new_entity(client)
    action =  { :type => "NetworkCreateEntity", 
               :name => "Player #{$pc}", 
               :mesh => "BetterShip.mesh", 
               :x => 0, :y => 0, :z => 0 
              }
         
    client.send_data(JSON.generate(action) + "\n")    
  end
  
  def send_create_yourself
    action =  { :type => "NetworkCreateYourself", 
               :name => "Player #{$pc}", 
               :mesh => "BetterShip.mesh", 
               :x => 0, :y => 0, :z => 0 
              }
         
    send_data(JSON.generate(action) + "\n")
  end
  
  def send_client_disconnect(c, dc)
    action = { :type => "NetworkDestroyEntity", :name => dc.player }
    c.socket.send_data(JSON.generate(action) + "\n")
  end
  
  def post_init
    $clients_list ||= {}
    
    # Remember the connection
    connection = Connection.new(self.object_id, self, "Player #{$pc}")
    $clients_list.merge!({self.object_id => connection})

    # Send this to message to the newly connected player
    send_create_yourself()
    
    # Alert all other players that a ship connected
    $clients_list.values.each do |c|
      send_new_entity(c.socket) if c.socket != self
    end
    
    $pc += 1
  end
  
  # Called when a client disconnects
  def unbind
    client = $clients_list.delete(self.object_id)
    
    $clients_list.values.each do |c|
      send_client_disconnect(c, client) if c.socket != self
    end
    
  end
  
  
  def receive_data(data)
    puts data
    send_data(data + "\n")
  end
end

EventMachine::run do
  host = '0.0.0.0'
  post = 8885
  EventMachine::start_server host, post, EchoServer
  puts "Started EchoServer on #{host}:#{post}..."

end


