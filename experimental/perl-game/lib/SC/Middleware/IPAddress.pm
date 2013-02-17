package SC::Middleware::IPAddress;

use parent qw( Plack::Middleware );

our $address;

sub call {
  my($self, $env) = @_;

  #print STDERR "Env keys: ", join(", ", keys(%$env)), "\n";
  #print STDERR "HTTP_X_FORWARDED_FOR: ", $env->{HTTP_X_FORWARDED_FOR}, "\n";
  #print STDERR "REMOTE_ADDR: ", $env->{REMOTE_ADDR}, "\n";
  $address = $env->{REMOTE_ADDR};
  if($address eq '127.0.0.1' && $env->{HTTP_X_FORWARDED_FOR}) {
    $address = $env->{HTTP_X_FORWARDED_FOR};
  }

  $self -> app -> ($env);
}

1;
