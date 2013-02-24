package SC::Handler;

use AnyEvent::Socket;
use Data::Dumper ();
use SC::MXP;

my %players;

sub connection_ip {
  my($self) = @_;

  if($self -> {remote_addr}) {
    $self -> {remote_addr};
  }
  else {
    "0.0.0.0";
  }
}

sub new {
  my $class = shift;

  $class = ref $class || $class;

  my $self = bless {} => $class;
  my %args = @_;


  $self -> {socket} = $args{socket} or die "No socket passed into constructor";
  $self -> {remote_addr} = $args{ip_address};

  # When we get a command from the player
  # but only if we've authenticated
  $self -> {socket} -> on(
    'parse' => sub {
      my($socket, $cmd, $cb) = @_;
      if($self->{name} && $self->conn) {
        $self->conn->push_write("$cmd\015");
      }
    }
  );

  $self -> {socket} -> on(
    'authenticate' => sub {
      my($socket, $username, $password) = @_;
      if($self->{name}) {
        # uh oh - we're already authenticated
      }
      else {
        # we want to initiate the connection to the game
        # and log in
        if($self->conn && $players{$username} && $players{$username}->{password} eq $password) {
          $self -> {name} = $username;
          #$players{$username} -> {mxp} = SC::MXP->new(
          #  socket => $self -> {socket},
          #);
          $players{$username} -> {mxp} -> setupConnection($self);
          $players{$username} -> {mxp} -> socket($self -> {socket});
          $self -> emit("authenticated");
        }
        else {
          $self -> {name} = $username;
          $players{$username} -> {mxp} = SC::MXP->new(
            socket => $self -> {socket},
          );
          $self -> setup_sockets( sub {
            # feed username and password to server and see if we connect
            $self -> conn -> push_write("$username\015");
            $self -> conn -> push_write("$password\015");
            # TODO: wait for confirmation from the MUD
            #$self -> emit("authenticated");
            $players{$username} -> {password} = $password;
          } );
        }
      }
    }
  );

  # create a new character - after going through all of the prompts in
  # the web interface
  $self -> {socket} -> on(
    'create' => sub {
      my($socket, $info, $cb) = @_;
      # we'll try to go through the process, but there might be problems
      #$self -> conn -> push_write("N\015");
      #$self -> conn -> push_write($info->{name} . "\015");
      #email, password2, capname, realname, password, name, gender
      $info->{gender} = lc $info->{gender};
      if($info->{password} ne $info->{password2}) {
        $self -> {socket} -> emit('create-error', "Passwords don't match.");
      }
      elsif($info->{email} =~ m{^\s*$}) {
        $self -> {socket} -> emit('create-error', "You must provide an email address.");
      }
      elsif($info->{name} =~ m{^\s*$}) {
        $self -> {socket} -> emit('create-error', "You must provide a character name.");
      }
      elsif($info->{gender} !~ m{^(fe)?male|neuter|none$}) {
        $self -> {socket} -> emit('create-error', "You must select a valid gender: male, female, neuter, or none.");
      }
      elsif($info->{ofage} ne 'yes') {
        $self -> {socket} -> emit('create-error', "You must be 13 years old or older to play Second Contract.");
      }
      else {
        $self -> {name} = "create-" . $info->{name};
        $players{$self->{name}} -> {mxp} = SC::MXP->new(
          socket => $self -> {socket}
        );
        $self -> setup_sockets( sub {
          $self -> conn -> push_write("N\015");
          my @info = @{$info}{qw(name capname gender realname email ofage password)};
          @info = map { s{:}{_:_}g; $_ } @info;
          $self -> conn -> push_write(join("::", @info) . "\015");
        });
      }
    }
  );

  $self -> {socket} -> on(
    'logout' => sub {
    }
  );

  $self -> emit("authenticate");

  $self;
}

sub conn {
  my($self) = @_;

  if($self->{name}) {
    $players{$self->{name}}->{handle};
  }
}

sub mxp {
  my($self) = @_;

  if($self -> {name}) {
    $players{$self->{name}}->{mxp};
  }
}

sub setup_conn {
  my($self, $conn) = @_;

  my $name = $self -> {name};
  my $handle; # avoid direct assignment so on_eof has it in scope.
  $handle = new AnyEvent::Handle
    fh     => $conn,
    on_error => sub {
      AE::log error => $_[2];
      $_[0]->destroy;
    },
    on_eof => sub {
      $handle->destroy; # destroy handle
      delete $players{$name}->{handle};
      delete $players{$name}->{mxp};
      #$self -> {socket} -> emit('authenticate');
      #$self -> {socket} -> close;
      AE::log info => "Done.";
    };
  $players{$name} -> {handle} = $handle;
  $players{$name} -> {mxp} -> setupConnection($self);
  $handle -> on_read( sub {
    if($players{$name} -> {mxp}) {
      my $line = $_[0]->rbuf;
      $_[0]->rbuf = "";
      $players{$name} -> {mxp} -> consume($line);
    }
  } );
}

sub setup_sockets {
  my($self, $cb) = @_;

  tcp_connect '127.0.0.1', 6666, sub {
    my($conn) = @_;
    if($conn) {
      $self -> setup_conn($conn);
      $cb -> ($conn);
    }
    else {
      # send back a 'can not connect' message
      $self -> emit( error => 'Unable to connect to game' );
    }
  };
}

sub emit { shift -> {socket} -> emit(@_) }

1;
