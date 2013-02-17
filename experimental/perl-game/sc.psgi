my $root;

BEGIN {
  use File::Basename ();
  use File::Spec     ();

  $root = File::Basename::dirname(__FILE__);
  $root = File::Spec->rel2abs($root);

  unshift @INC, "$root/lib";
}

use PocketIO;
use Plack::Builder;
use Plack::App::File;
use Plack::Middleware::Static;
use SC::Handler;
use SC::Middleware::IPAddress;

builder {
  mount '/socket.io/socket.io.js' =>
    Plack::App::File->new(file => "$root/public/socket.io.js");

  mount '/socket.io/static/flashsocket/WebSocketMain.swf' =>
    Plack::App::File->new(file => "$root/public/WebSocketMain.swf");

  mount '/socket.io/static/flashsocket/WebSocketMainInsecure.swf' =>
    Plack::App::File->new(file => "$root/public/WebSocketMainInsecure.swf");

  mount '/socket.io' => builder {
    enable "+SC::Middleware::IPAddress";
    PocketIO -> new(
      handler => sub {
        my $self = shift;

        my $translator = SC::Handler -> new(
          socket => $self,
          ip_address => $SC::Middleware::IPAddress::address,
        );
      }
    );
  };

  mount '/playing/game/' => builder {
    enable 'Static',
      path => qr/\.(?:js|css|jpe?g|gif|png|html?|swf|ico)$/,
      root => "$root/static";

    enable "SimpleLogger", level => 'debug';

    #my $html = do {
    #  local $/;
    #  open my $fh, '<', "$root/public/game.html"
    #    or die $!;
    #  <$fh>;
    #};
    sub {
      my $html = do {
        local $/;
        open my $fh, '<', "$root/public/game.html"
          or die $!;
        <$fh>;
      };
      [  200,
         [ 'Content-Type' => 'text/html',
           'Content-Length' => length($html),
         ],
         [$html]
      ];
    };
  };
};
