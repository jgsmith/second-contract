package SC::MXP;

use feature qw( switch );
use strict;
use warnings;

use HTML::Entities;

my $chIAC = chr(255);
my $chDONT = chr(254);
my $chDO = chr(253);
my $chWILL = chr(251);
my $chSB = chr(250);
my $chSE = chr(240);
my $chOptMXP = chr(91);
my $chOptENVIRON = chr(39);
my $chOptLINEMODE = chr(34);
my $chOptECHO = chr(1);
my $chESCLB = chr(27) . "[";
my $chIS= chr(0);
my $chSEND = chr(1);
my $chINFO = chr(2);
my $chVAR = chr(0);
my $chVALUE = chr(1);
my $chUSERVAR = chr(3);
my $chMODE = chr(1);
my $chMODEEDIT = chr(1);


sub new {
  my $class = shift;

  $class = ref $class || $class;

  my $self = bless {} => $class;

  my %args = @_;

  $self->{socket} = $args{socket};
  $self -> {_can_send} = 0;
  $self -> {_buffer} = "";
  $self -> {_in_mxp_setup} = 0;
  $self -> {_done_mxp_setup} = 0;
  $self -> event_reset;

  $self;
}

sub socket {
  my $self = shift;

  if(@_) {
    $self -> {socket} = shift;
  }
  $self -> {socket};
}

# We want to extract all of the useful stuff from the stream and package it
# up as a set of JSON messages
sub event_reset {
  my($self) = @_;
  $self -> {_color} = "default";
  $self -> {_bgcolor} = "transparent";
  $self -> {_blink} = 0;
  $self -> {_bold} = 0;
  $self -> {_underline} = 0;
  $self -> {_chars} = "";
}

sub event_bold { shift -> {_bold} = 1; }
sub event_blink { shift -> {_blink} = 1; }
sub event_underline { shift -> {_underline} = 1; }
  
sub flush_characters {
  my($self) = @_;

  if($self -> {_chars} ne "") {
    my $span;
    if($self -> {_ignore_style}) {
      $span = $self -> {_chars};
    }
    else {
      $span = "<span style='";
      if($self -> {_bold}) {
        $span .= "font-weight: bold;";
      }
      if($self -> {_underline}) {
        $span .= "font-style: italic;";
      }
      if($self -> {_blink}) {
        $span .= "text-decoration: blink;";
      }
      if($self -> {_color} && $self -> {_color} ne 'default') {
        $span .= "color: " . $self->{_color} . ";";
      }
      if($self -> {_bgcolor} && $self -> {_bgcolor} ne 'transparent') {
        $span .= "background-color: " . $self -> {_bgcolor} . ";"
      }
      $span .= "'>" . ($self -> {_chars}) . "</span>";
      $self -> {_chars} = "";
    }

    if($self -> {_capture}) {
      $self -> {_captured} .= $span;
    }
    else {
      $self -> {socket} -> emit("text", $span) if $self -> {_can_send};
    }
  }
}

sub event_characters {
  my($self, $c) = @_;

  $self -> {_chars} .= $c;
}

sub event_newline {
  my($self) = @_;

  $self -> {_chars} .= "<br/>";
  #$self -> {socket} -> emit("text", "<br/>") if $self -> {_can_send};
}

sub event_foreground {
  my($self, $color) = @_;
  $self -> {_color} = $color;
}

sub event_background {
  my($self, $color) = @_;
  $self -> {_bgcolor} = $color;
}

my %vars = map { $_ => 1 } qw(
  cname class level xp maxxp sp maxsp hp maxhp credit creditUsed
  food drink caffeine poison alcohol
  agility charisma coordination durability intelligence luck
  speed strength wisdom
  heartrate healing position
  carried-mass max-carried-mass
);

my %structs = map { $_ => 1 } qw(
  head torso neck
  left-arm right-arm left-leg right-leg
  left-hand right-hand left-foot right-foot
);

my %room = qw(
  RName    name
  RExits   exits
  RDesc    description
  RWeather weather
  RTouch   touch
  RSmell   smell
  RSound   sound
);

sub event_startElement {
  my($self, $local, $attrs) = @_;
  if($vars{$local} || $room{$local}) {
    $self -> flush_characters;
    $self -> {_capture} = 1;
    $self -> {_captured} = "";
    return;
  }
  if($structs{$local}) {
    $self -> flush_characters;
    $self -> {socket} -> emit($local, $attrs);
    return;
  }
  given($local) {
    when("Pages") {
      $self -> {_pages} = [];
    }
    when("Page") {
      $self -> {_page} = [];
    }
    when("Line") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
    }
    when("Created") {
      $self -> {socket} -> emit("created");
      $self -> {_can_send} = 1;
    }
    when("Quit") {
      $self -> flush_characters;
      $self -> {socket} -> emit("Quit");
      $self -> {handle} -> push_shutdown;
    }
    when("Authenticated") {
      $self -> {socket} -> emit("authenticated");
      $self -> {_can_send} = 1;
    }
    when("prompt") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
    }
    when("CreateError") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
    }
    when("AuthError") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
    }
    when("News") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
    }
    when("Buy") {
      $self -> flush_characters;
      #$self -> {_capture} = 1;
      #$self -> {_captured} = "";
      #$self -> {_buy} = $attrs;
    }
    when("Skill") {
      $self -> flush_characters;
      $self -> {socket} -> emit('skill', $attrs);
    }
    when('Ability') {
      $self -> flush_characters;
      $self -> {socket} -> emit('ability', $attrs);
    }
    when("Room") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
      $self -> {room} = {};
    }
    when("Ex") {
      $self -> flush_characters;
      $self -> {_exit} = $attrs->{dir};
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
    }
    when("ExName") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
    }
    when("RItems") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
    }
    when("RNPCs") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
    }
    when("Combat") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
    }
    when("MMap") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
    }
    when("Item") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_ignore_style} = 0;
      $self -> {_captured} = "";
      $self -> {_item} = $attrs;
    }
    when("Emote") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
      $self -> {_emote} = $attrs;
    }
    when("Env") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
    }
    when("Chat") {
      $self -> flush_characters;
      $self -> {_capture} = 1;
      $self -> {_captured} = "";
      $self -> {_chat} = $attrs;
    }
  }
}

sub event_endElement {
  my($self, $local) = @_;
  if($vars{$local}) {
    $self -> flush_characters;
    my $sp = $self -> {_captured};
    # strip out any html
    $sp =~ s{<.*?>}{}g;
    $self -> {socket} -> emit($local, $sp) if $self -> {_can_send};
    $self -> {_capture} = 0;
    $self -> {_captured} = "";
    return;
  }
  if($room{$local}) {
      $self -> flush_characters;
      $self -> {room} -> {$room{$local}} = $self -> {_captured};
      $self -> {_captured} = "";
      return;
  }
  given($local) {
    when("Pages") {
      $self -> {socket} -> emit("page", [ map { @{$_} } @{$self -> {_pages}} ]);
      delete $self -> {_pages};
    }
    when("Page") {
      if($self -> {_pages}) {
        push @{$self -> {_pages}}, $self -> {_page};
      }
      else {
        $self -> {socket} -> emit("page", $self -> {_page});
      }
      delete $self -> {_page};
    }
    when("Line") {
      $self -> flush_characters;
      push @{$self -> {_page}}, $self -> {_captured};
      $self -> {_capture} = 0;
      $self -> {_captured} = "";
    }
    when("prompt") {
      $self -> flush_characters;
      $self -> {_capture} = 0;
      $self -> {_captured} = "";
    }
    when("CreateError") {
      $self -> flush_characters;
      $self -> {socket} -> emit('create-error', $self -> {_captured});
      $self -> {_capture} = 0;
      $self -> {_captured} = "";
      $self -> {handle} -> push_shutdown;
    }
    when("AuthError") {
      $self -> flush_characters;
      $self -> {socket} -> emit('auth-error', $self -> {_captured});
      $self -> {_capture} = 0;
      $self -> {_captured} = "";
      $self -> {handle} -> push_shutdown;
    }
    when("Buy") {
      $self -> flush_characters;
      #$self -> {socket} -> emit('news', $self -> {_captured});
      #$self -> {_capture} = 0;
      #$self -> {_captured} = "";
    }
    when("News") {
      $self -> flush_characters;
      $self -> {socket} -> emit('news', $self -> {_captured});
      $self -> {_capture} = 0;
      $self -> {_captured} = "";
    }
    when("Room") {
      $self -> flush_characters;
      $self -> {socket} -> emit("room", $self -> {room}) if $self -> {_can_send};
      $self -> {_capture} = 0;
      $self -> {_captured} = "";
    }
    when("Ex") {
      $self -> flush_characters;
      $self -> {room} -> {"exit"} -> {$self -> {_exit}} -> {name} = $self -> {_captured};
      $self -> {_captured} = "";
    }
    when("ExName") {
      $self -> flush_characters;
      $self -> {room} -> {"exit"} -> {$self -> {_exit}} -> {title} = $self -> {_captured};
      $self -> {_captured} = "";
    }
    when("RItems") {
      $self -> flush_characters;
      $self -> {room} -> {items} = $self -> {_captured};
      $self -> {_captured} = "";
    }
    when("RNPCs") {
      $self -> flush_characters;
      $self -> {room} -> {npcs} = $self -> {_captured};
      $self -> {_captured} = "";
    }
    when("Item") {
      $self -> flush_characters;
      my $nom = $self -> {_captured};
      $self -> {_captured} = "";
      $self -> {_capture} = 0;
      $self -> {_ignore_style} = 0;
      $self -> {socket} -> emit("item", {
        %{$self -> {_item}},
        label => $nom,
      });
    }
    when("Emote") {
      $self -> flush_characters;
      my $msg = $self -> {_captured};
      $self -> {socket} -> emit("emote", {
        channel => $self -> {_emote} -> {chan},
        speaker => $self -> {_emote} -> {who},
        text => $msg,
      });
      $self -> {_captured} = "";
      $self -> {_capture} = 0;
    }
    when("Chat") {
      $self -> flush_characters;
      my $msg = $self -> {_captured};
      $self -> {socket} -> emit("chat", {
        channel => $self -> {_chat} -> {chan},
        speaker => $self -> {_chat} -> {who},
        text => $msg,
      });
      $self -> {_captured} = "";
      $self -> {_capture} = 0;
    }
    when("MMap") {
      $self -> flush_characters;
      my $mmap = $self -> {_captured};
      $mmap =~ s{--------\|}{--------<br>\|};
      $mmap =~ m{^(<span.*?>)};
      my $span = $1;
      my(@lines) = ($mmap =~ s{\|([^|-]+?)</span}{|</span}g);
      #if(@lines) {
      #  $mmap .= $span . join("</span>$span", @lines) . "</span>";
      #}
      $self -> {room} -> {minimap} = $mmap;
      $self -> {_captured} = "";
    }
    when("Combat") {
      $self -> flush_characters;
      $self -> {socket} -> emit("combat", $self -> {_captured}) if $self -> {_can_send};
      $self -> {_captured} = "";
      $self -> {_capture} = 0;
    }
    when("Env") {
      $self -> flush_characters;
      $self -> {socket} -> emit("env", $self -> {_captured}) if $self -> {_can_send};
      $self -> {_captured} = "";
      $self -> {_capture} = 0;
    }
  }
}


my %colors = qw(
  30 back
  31 red
  32 green
  33 yellow
  34 blue
  35 magenta
  36 cyan
  37 white
  39 default
  40 black
  41 red
  42 green
  43 yellow
  44 blue
  45 magenta
  46 cyan
  47 white
  49 transparent
);
# We emit messages as we can based on the contents of the lines we consume
sub consume {
  my($self, $line) = @_;

  # now process as much of the buffer as we can, reliably -- until we get to
  # a newline or a close tag, for example

  # we need to translate colors and blink tags
  #$line = encode_entities($line, '^\n\x20-\x25\x27-\x7e');
  return if !defined($line) || $line eq "";
  $line = $self -> {_buffer} . $line;
  my $matched = 1;
  my $ignore_newline = 0;
  my $last_pos = -1;
  if(!$self->{_done_mxp_setup}) {
    if($self->{_in_mxp_setup}) {
      if($line =~ m{<!-- END MXP setup -->(.*)$}s) {
         $line = $1;
         $self -> {_in_mxp_setup} = 0;
         $self -> {_done_mxp_setup} = 1;
      }
      else {
         $self -> {_buffer} = "";
         return;
      }
    }
    elsif($line =~ m{^(.*)<!-- MXP setup -->(.*)$}s) {
      $line = $1;
      my $bit = $2;
      if($bit =~ m{<!-- END MXP setup -->(.*)$}s) {
        $line .= $1;
        $self -> {_in_mxp_setup} = 0;
        $self -> {_done_mxp_setup} = 1;
      }
      else {
        $self -> {_in_mxp_setup} = 1;
      }
    }
  }
  while($matched) {
    $last_pos = pos($line);
    $matched = 0;
    while($line =~ m{\G(\012\015?|\015\012?)}sgc) {
      if($ignore_newline) {
        $ignore_newline = 0;
      }
      else {
        $self -> flush_characters();
        $self -> event_newline();
      }
      $matched = 1;
    }
    next if $matched;
    while($line =~ m{\G</([^>\s]+?)>}sgc) {
      $matched = 1;
      $ignore_newline = 1;
      $self -> event_endElement($1);
    }
    while($line =~ m{\G<!--.*?-->}sgc) {
      $matched = 1;
      $ignore_newline = 1;
    }
    next if $matched;
    $last_pos = pos($line);
    while($line =~ m{\G<!(ELEMENT|ATT|ENTITY)\s+\S+\s*}sgc) {
      $matched = 1;
      $ignore_newline = 1;
      $line =~ m{\G(\s*((".*?"|[^=\s>]+?=)?('.*?'|".*?"))\s*)*}sgc;
      if($line =~ m{\G\s*>}sgc) {
        $matched = 1;
        $last_pos = pos($line);
      }
      else {
        pos($line) = $last_pos;
      }
    }
    next if $matched;
    $last_pos = pos($line);
    while($line =~ m{\G<([^/\s>]+)\s*}sgc) {
      my $localName = $1;
      $ignore_newline = 1;
      my %attrs;
      while($line =~ m{\G\s*([^\s=>]+)(?:=(?:"(.*?)"|(\S+)))?\s*}sgc) {
        if(defined($2) || defined($3)) {
          $attrs{$1} = defined($2) ? $2 : $3;
        }
        else {
          $attrs{lc $localName} = $1;
        }
      }
      if($line =~ m{\G\s*>}sgc) {
        $last_pos = pos($line);
        $matched = 1;
        $self -> event_startElement($localName, \%attrs);
      }
      elsif($line =~ m{\G\s*/>}sgc) {
        $last_pos = pos($line);
        $matched = 1;
        $self -> event_startElement($localName, \%attrs);
        $self -> event_endElement($localName);
      }
      else {
        pos($line) = $last_pos;
        $matched = 0;
        last;
      }
    }
    next if $matched;
    while($line =~ m{\G([ -;=-~]+)}sgc) {
      $self -> event_characters($1);
      $matched = 1;
    }
    next if $matched;
    if($line =~ m{\G(\033\[\??\d+[\@ABCDEFGIJKLMPSTXZ])+}sgc) {
      $matched = 1;
    }
    if($line =~ m{\G(\033\[\d+;\d+H)+}sgc) {
      $matched = 1;
    }
    next if $matched;
    while($line =~ m{\G\033\[(\d+)m}sgc) {
      $matched = 1;
      my $idx = $1;
      if($colors{$idx}) {
        $self -> flush_characters();
        if($idx gt "39") {
          $self -> event_background($colors{$idx});
        }
        else {
          $self -> event_foreground($colors{$idx});
        }
      }
      else {
        given($idx) {
          when('0') {
            $self -> flush_characters();
            $self -> event_reset();
          }
          when('1') {
            $self -> flush_characters();
            $self -> event_bold();
          }
          when('4') {
            $self -> flush_characters();
            $self -> event_underline();
          }
          when('5') {
            $self -> flush_characters();
            $self -> event_blinking();
          }
        }
      }
    }
    next if $matched;
    my @vars;
    while(@vars = $line =~ m{\G\377\372\044\001(?:\000([ -~]+)|(\003))\377\360}sgc) {
      # we only care about "USER" right now
      $self -> {handle} -> push_write($chIAC . $chSB . $chOptENVIRON . $chIS);
      for my $v (grep { defined } @vars) {
        if($v eq 'USER') { # spit back IP address
          $self -> {handle} -> push_write($chVAR . "USER" . $chVALUE . "255.255.255.255");
        }
        elsif($v eq $chUSERVAR) { # spit back IP address as "USER"
          $self -> {handle} -> push_write($chUSERVAR . "USER" . $chVALUE . "255.255.255.255");
        }
      }
      $self -> {handle} -> push_write($chIAC . $chSE);
      $matched = 1;
    }
    next if $matched;
    if($line =~ m{\G[^ -~]}sgc) {
      #$self -> event_characters("?");
      $matched = 1;
    }
  }
  
  $self -> {_buffer} = defined(pos($line)) ? substr($line, pos($line)) : $line;
  #$self -> {socket} -> emit("text", $line);

}

sub setupConnection {
  my($self, $handler) = @_;

  # we want to make sure the MXP option is turned on
  $self -> {handle} = $handler -> conn;
  my $ip = $handler -> connection_ip;
  
  $self -> {handle} -> push_write($chIAC . $chWILL . $chOptMXP);
  $self -> {handle} -> push_write($chIAC . $chWILL . $chOptENVIRON);
  $self -> {handle} -> push_write($chIAC . $chWILL . $chOptLINEMODE);
  #$self -> {handle} -> push_write(
  #   $chIAC . $chSB . $chOptLINEMODE . $chMODE . $chMODEEDIT . $chIAC . $chSE
  #);
  $self -> {handle} -> push_write($chIAC . $chDONT . $chOptECHO);
  $self -> {handle} -> push_write($chIAC . $chSB . $chOptENVIRON . $chIS);
  $self -> {handle} -> push_write($chVAR . "USER" . $chVALUE . "JS" . $ip);
  #$self -> {handle} -> push_write($chUSERVAR . "USER" . $chVALUE . "255.255.255.255");
  $self -> {handle} -> push_write($chIAC . $chSE);
}

sub print {
  my $self = shift;

  $self -> {handle} -> push_write(join("",@_));
}

1;
