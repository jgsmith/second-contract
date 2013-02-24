(function() {
  var __slice = Array.prototype.slice,
    __indexOf = Array.prototype.indexOf || function(item) { for (var i = 0, l = this.length; i < l; i++) { if (i in this && this[i] === item) return i; } return -1; };

  $(function() {
    var abilities, abilityInfoEl, app, channelChat, combatChat, cs, debugChat, defaultChat, el, invEl, inventoryContainers, lens, otherSkills, playerInventory, pp, primarySkills, s, secondarySkills, skillLens, type, _fn, _fn2, _fn3, _i, _j, _k, _len, _len2, _len3, _ref, _ref2, _ref3, _results;
    $('#login-form').modal({
      backdrop: 'static',
      keyboard: false,
      show: false
    });
    $('#login-form').on('show', function() {
      return $('#login-form-error').hide();
    });
    $('#login-form').on('shown', function() {
      return $('#inputUsername').focus();
    });
    $('#login-submit').click(function() {
      $('#login-form-error').hide();
      app.emit('authenticate', $('#inputUsername').val(), $('#inputPassword').val());
      return false;
    });
    $('#new-char-form').modal({
      keyboard: true,
      show: false
    });
    $('#new-char-form').on('show', function() {
      return $('#new-char-form-error').hide();
    });
    $('#new-char-form').on('shown', function() {
      return $('#newUsername').focus();
    });
    $('#new-character').click(function() {
      $('#login-form').modal('hide');
      $('#new-char-form').modal('show');
      return false;
    });
    $('#new-char-cancel').click(function() {
      $('#new-char-form').modal('hide');
      $('#login-form').modal('show');
      return false;
    });
    $('#new-char-submit').click(function() {
      $('#new-char-form-error').hide();
      app.emit('create', {
        name: $('#newUsername').val(),
        capname: $('#newCapName').val(),
        password: $('#newPassword').val(),
        password2: $('#newPassword2').val(),
        gender: $('#newGender').val(),
        realname: $('#newRealName').val(),
        ofage: $('#newAgeCheck').val(),
        email: $('#newEmail').val()
      });
      return false;
    });
    $('#news-display-close').click(function() {
      $('#news-display').modal('hide');
      return false;
    });
    $('#news-display').modal({
      show: false
    });
    $('#page-display-close').click(function() {
      $('#page-display').modal('hide');
      return false;
    });
    $('#page-display').modal({
      show: false
    });
    $('#send-command').submit(function() {
      var cmd;
      cmd = $('#cmd-line').val();
      app.emit('parse', $('#cmd-line').val());
      $('#cmd-line').focus();
      $('#cmd-line').select();
      return false;
    });
    MITHGrid.defaults('SecondContract.Component.StatusDisplay', {
      variables: {
        Status: {
          is: 'rw'
        }
      }
    });
    MITHGrid.defaults('SecondContract.Component.SimpleStatistic', {
      variables: {
        Value: {
          is: 'rw'
        },
        MaxValue: {
          is: 'rw'
        }
      }
    });
    MITHGrid.defaults('SecondContract.Component.PercentProgress', {
      variables: {
        Value: {
          is: 'rw'
        },
        MaxValue: {
          is: 'rw',
          "default": 100
        }
      }
    });
    MITHGrid.defaults('SecondContract.App', {
      dataStores: {
        char: {},
        text: {},
        inventory: {}
      },
      variables: {
        ConnectionStatus: {
          is: 'rw'
        },
        Name: {
          is: 'rw'
        },
        Level: {
          is: 'rw'
        },
        Position: {
          is: 'rw'
        },
        Cardinality: {
          is: 'rw'
        },
        Credit: {
          is: 'rw'
        },
        CreditUsed: {
          is: 'rw'
        },
        Environment: {
          is: 'rw'
        },
        CarriedMass: {
          is: 'rw'
        },
        MaxCarriedMass: {
          is: 'rw'
        },
        HP: {
          is: 'rw'
        },
        MaxHP: {
          is: 'rw'
        },
        SP: {
          is: 'rw'
        },
        MaxSP: {
          is: 'rw'
        },
        XP: {
          is: 'rw'
        },
        MaxXP: {
          is: 'rw'
        },
        Food: {
          is: 'rw'
        },
        Drink: {
          is: 'rw'
        },
        Alcohol: {
          is: 'rw'
        },
        Caffeine: {
          is: 'rw'
        },
        Poison: {
          is: 'rw'
        },
        Agility: {
          is: 'rw'
        },
        Charisma: {
          is: 'rw'
        },
        Coordination: {
          is: 'rw'
        },
        Durability: {
          is: 'rw'
        },
        Intelligence: {
          is: 'rw'
        },
        Luck: {
          is: 'rw'
        },
        Speed: {
          is: 'rw'
        },
        Strength: {
          is: 'rw'
        },
        Wisdom: {
          is: 'rw'
        },
        HeartRate: {
          is: 'rw'
        },
        HealRate: {
          is: 'rw'
        },
        Head: {
          is: 'rw'
        },
        MaxHead: {
          is: 'rw'
        },
        Neck: {
          is: 'rw'
        },
        MaxNeck: {
          is: 'rw'
        },
        Torso: {
          is: 'rw'
        },
        MaxTorso: {
          is: 'rw'
        },
        LeftArm: {
          is: 'rw'
        },
        MaxLeftArm: {
          is: 'rw'
        },
        LeftLeg: {
          is: 'rw'
        },
        MaxLeftLeg: {
          is: 'rw'
        },
        RightArm: {
          is: 'rw'
        },
        MaxRightArm: {
          is: 'rw'
        },
        RightLeg: {
          is: 'rw'
        },
        MaxRightLeg: {
          is: 'rw'
        },
        LeftHand: {
          is: 'rw'
        },
        MaxLeftHand: {
          is: 'rw'
        },
        RightHand: {
          is: 'rw'
        },
        MaxRightHand: {
          is: 'rw'
        },
        LeftFoot: {
          is: 'rw'
        },
        MaxLeftFoot: {
          is: 'rw'
        },
        RightFoot: {
          is: 'rw'
        },
        MaxRightFoot: {
          is: 'rw'
        }
      }
    });
    MITHGrid.globalNamespace('SecondContract', function(SecondContract) {
      SecondContract.namespace('Component', function(Component) {
        Component.namespace('StatusDisplay', function(CS) {
          return CS.initInstance = function() {
            var args;
            args = 1 <= arguments.length ? __slice.call(arguments, 0) : [];
            return MITHGrid.initInstance.apply(MITHGrid, ['SecondContract.Component.StatusDisplay'].concat(__slice.call(args), [function(that, container) {
              var options;
              options = that.options;
              return that.onDestroy(that.events.onStatusChange.addListener(function(s) {
                $(container).attr("class", "");
                $(container).text('');
                if (options.status[s] != null) {
                  $(container).addClass(options.status[s]);
                  return $(container).text(s);
                }
              }));
            }]));
          };
        });
        Component.namespace('SimpleStatistic', function(SS) {
          return SS.initInstance = function() {
            var args;
            args = 1 <= arguments.length ? __slice.call(arguments, 0) : [];
            return MITHGrid.initInstance.apply(MITHGrid, ['SecondContract.Component.SimpleStatistic'].concat(__slice.call(args), [function(that, container) {
              var options;
              options = that.options;
              that.onDestroy(that.events.onValueChange.addListener(function(v) {
                if (v > that.getMaxValue()) v = that.getMaxValue();
                return $(container).text(v);
              }));
              return that.onDestroy(that.events.onMaxValueChange.addListener(function(v) {
                if (v > that.getValue()) v = that.getValue();
                return $(container).text(v);
              }));
            }]));
          };
        });
        return Component.namespace('PercentProgress', function(PP) {
          return PP.initInstance = function() {
            var args;
            args = 1 <= arguments.length ? __slice.call(arguments, 0) : [];
            return MITHGrid.initInstance.apply(MITHGrid, ['SecondContract.Component.PercentProgress'].concat(__slice.call(args), [function(that, container) {
              var drawBar, options;
              options = that.options;
              drawBar = function() {
                var m, v;
                v = parseInt(that.getValue(), 10);
                m = parseInt(that.getMaxValue(), 10);
                if (m === 0) m = 1;
                if (v > m) v = m;
                v = parseInt(100 * v / m, 10);
                if (options.inverted) v = 100 - v;
                $(container).css("width", v + "%");
                if (options.asPercent) {
                  return $(container).attr("title", v + " / 100");
                } else {
                  if (options.inverted) {
                    return $(container).attr("title", (that.getMaxValue() - that.getValue()) + " / " + that.getMaxValue());
                  } else {
                    return $(container).attr("title", that.getValue() + " / " + that.getMaxValue());
                  }
                }
              };
              that.onDestroy(that.events.onValueChange.addListener(drawBar));
              return that.onDestroy(that.events.onMaxValueChange.addListener(drawBar));
            }]));
          };
        });
      });
      return SecondContract.namespace('App', function(App) {
        return App.initInstance = function() {
          var args, _ref;
          args = 1 <= arguments.length ? __slice.call(arguments, 0) : [];
          return (_ref = MITHGrid.Application).initInstance.apply(_ref, ['SecondContract.App'].concat(__slice.call(args), [function(that, container) {
            var authenticated, chatId, checkFocus, combatId, envId, live_connection, options, socket, textId;
            live_connection = false;
            socket = io.connect();
            authenticated = false;
            options = that.options;
            that.emit = function() {
              var args;
              args = 1 <= arguments.length ? __slice.call(arguments, 0) : [];
              return socket.emit.apply(socket, args);
            };
            checkFocus = function() {
              if (authenticated) $('#cmd-line').focus();
              return setTimeout(checkFocus, 1);
            };
            checkFocus();
            socket.on('connect', function() {
              return that.setConnectionStatus("Connected");
            });
            socket.on('reconnect', function() {
              return that.setConnectionStatus("Connected");
            });
            socket.on('reconnecting', function() {
              return that.setConnectionStatus("Reconnecting");
            });
            socket.on('Quit', function() {
              that.setConnectionStatus("Disconnected");
              $('#inputUsername').val("");
              $('#inputPassword').val("");
              authenticated = false;
              return live_connection = false;
            });
            socket.on('level', that.setLevel);
            socket.on('class', that.setCardinality);
            socket.on('cname', that.setName);
            socket.on('hp', that.setHP);
            socket.on('maxhp', that.setMaxHP);
            socket.on('sp', that.setSP);
            socket.on('maxsp', that.setMaxSP);
            socket.on('xp', that.setXP);
            socket.on('maxxp', that.setMaxXP);
            socket.on('carried-mass', function(x) {
              return that.setCarriedMass(x / 20);
            });
            socket.on('max-carried-mass', function(x) {
              return that.setMaxCarriedMass(x / 20);
            });
            socket.on('credit', that.setCredit);
            socket.on('creditUsed', that.setCreditUsed);
            socket.on('position', that.setPosition);
            socket.on('food', that.setFood);
            socket.on('drink', that.setDrink);
            socket.on('alcohol', that.setAlcohol);
            socket.on('caffein', that.setCaffeine);
            socket.on('poison', that.setPoison);
            socket.on('agility', that.setAgility);
            socket.on('charisma', that.setCharisma);
            socket.on('coordination', that.setCoordination);
            socket.on('durability', that.setDurability);
            socket.on('intelligence', that.setIntelligence);
            socket.on('luck', that.setLuck);
            socket.on('speed', that.setSpeed);
            socket.on('strength', that.setStrength);
            socket.on('wisdom', that.setWisdom);
            socket.on('heartrate', that.setHeartRate);
            socket.on('healing', that.setHealRate);
            socket.on('head', function(v) {
              that.setHead(v.hp);
              return that.setMaxHead(v.maxhp);
            });
            socket.on('neck', function(v) {
              that.setNeck(v.hp);
              return that.setMaxNeck(v.maxhp);
            });
            socket.on('torso', function(v) {
              that.setTorso(v.hp);
              return that.setMaxTorso(v.maxhp);
            });
            socket.on('left-arm', function(v) {
              that.setLeftArm(v.hp);
              return that.setMaxLeftArm(v.maxhp);
            });
            socket.on('left-leg', function(v) {
              that.setLeftLeg(v.hp);
              return that.setMaxLeftLeg(v.maxhp);
            });
            socket.on('right-arm', function(v) {
              that.setRightArm(v.hp);
              return that.setMaxRightArm(v.maxhp);
            });
            socket.on('right-leg', function(v) {
              that.setRightLeg(v.hp);
              return that.setMaxRightLeg(v.maxhp);
            });
            socket.on('left-hand', function(v) {
              that.setLeftHand(v.hp);
              return that.setMaxLeftHand(v.maxhp);
            });
            socket.on('left-foot', function(v) {
              that.setLeftFoot(v.hp);
              return that.setMaxLeftFoot(v.maxhp);
            });
            socket.on('right-hand', function(v) {
              that.setRightHand(v.hp);
              return that.setMaxRightHand(v.maxhp);
            });
            socket.on('right-foot', function(v) {
              that.setRightFoot(v.hp);
              return that.setMaxRightFoot(v.maxhp);
            });
            socket.on('item', function(msg) {
              var f, i, item;
              if (msg.env === "FURNACE") {
                return that.dataStore.char.removeItems([msg.id]);
              } else {
                item = that.dataStore.char.getItem(msg.id);
                if (item.type != null) {
                  f = that.dataStore.char.updateItems;
                } else {
                  f = that.dataStore.char.loadItems;
                }
                i = {
                  id: msg.id,
                  label: msg.label,
                  name: msg.keyame,
                  type: msg.type != null ? [msg.type, "Item"] : ["Item"],
                  wielded: msg.wielded === "1",
                  container: msg.container === "1",
                  worn: msg.worn === "1",
                  env: msg.env
                };
                return f([i]);
              }
            });
            socket.on('ability', function(msg) {
              var f, item;
              item = that.dataStore.char.getItem("ability-" + msg.name);
              if (item.type != null) {
                f = that.dataStore.char.updateItems;
              } else {
                f = that.dataStore.char.loadItems;
              }
              return f([
                {
                  id: "ability-" + msg.name,
                  type: "Ability",
                  name: msg.name,
                  level: parseInt(msg.level, 10),
                  maxlevel: parseInt(msg.maxlevel, 10)
                }
              ]);
            });
            socket.on('skill', function(msg) {
              var f, item;
              item = that.dataStore.char.getItem("skill-" + msg.name);
              if (item.type != null) {
                f = that.dataStore.char.updateItems;
              } else {
                f = that.dataStore.char.loadItems;
              }
              return f([
                {
                  id: "skill-" + msg.name,
                  name: msg.name,
                  type: "Skill",
                  level: parseInt(msg.level, 10),
                  maxlevel: parseInt(msg.maxlevel, 10),
                  "class": parseInt(msg["class"], 10),
                  points: parseInt(msg.points, 10),
                  maxpoints: parseInt(msg.maxpoints, 10)
                }
              ]);
            });
            combatId = 0;
            socket.on('combat', function(m) {
              that.dataStore.text.loadItems([
                {
                  id: "combat-" + combatId,
                  type: "Combat",
                  text: m
                }
              ]);
              return combatId += 1;
            });
            textId = 0;
            socket.on('text', function(m) {
              that.dataStore.text.loadItems([
                {
                  id: "text-" + textId,
                  type: "Debug",
                  text: m
                }
              ]);
              return textId += 1;
            });
            envId = 0;
            socket.on('env', function(m) {
              that.dataStore.text.loadItems([
                {
                  id: "env-" + envId,
                  type: "Text",
                  text: m
                }
              ]);
              return envId += 1;
            });
            chatId = 0;
            socket.on('chat', function(m) {
              that.dataStore.text.loadItems([
                {
                  id: "chat-" + chatId,
                  type: "Chat",
                  channel: m.channel,
                  speaker: m.speaker,
                  text: m.text
                }
              ]);
              return chatId += 1;
            });
            socket.on('emote', function(m) {
              that.dataStore.text.loadItems([
                {
                  id: "emote-" + chatId,
                  type: "Emote",
                  channel: m.channel,
                  speaker: m.speaker,
                  text: m.text
                }
              ]);
              return chatId += 1;
            });
            $('#outputTabs a:first').tab('show');
            $('#charTabs a:first').tab('show');
            socket.on('news', function(n) {
              $('#news-display-body').html(n);
              return $('#news-display').modal('show');
            });
            socket.on('page', function(lines) {
              var title;
              title = lines.shift();
              $('#page-title').html(title);
              $('#page-display-body').html(lines.join("<br>"));
              return $('#page-display').modal('show');
            });
            socket.on('room', function(msg) {
              var einfo, html, k, _ref;
              if (msg.id != null) that.setEnvironment(msg.id);
              $('#room-name').html(msg.name || "");
              $('#room-description').html(msg.description || "");
              if (msg.minimap != null) {
                $('#room-minimap').html(msg.minimap);
                $('#room-minimap').show();
              } else {
                $('#room-minimap').hide();
              }
              $('#room-weather').html(msg.weather || "");
              $('#room-items').html(msg.items || "");
              $('#room-npcs').html(msg.npcs || "");
              $('#room-senses').html((msg.sound || "") + " " + (msg.smell || "") + " " + (msg.touch || ""));
              html = "";
              if (msg.exit != null) {
                _ref = msg.exit;
                for (k in _ref) {
                  einfo = _ref[k];
                  if (msg.exit.hasOwnProperty(k)) {
                    html += "&nbsp;&nbsp;&nbsp;" + einfo.name + " [" + einfo.title + "]<br/>";
                  }
                }
                if (html !== "") html = "Obvious exits:<br/>" + html;
              }
              return $('#room-exits').html(html);
            });
            socket.on('authenticate', function(e) {
              that.setConnectionStatus('Connecting');
              if ($('#inputUsername').val() !== "" && $('#inputPassword').val() !== "") {
                return socket.emit('authenticate', $('#inputUsername').val(), $('#inputPassword').val());
              } else {
                $('#login-form').modal('show');
                return $('#inputUsername').focus();
              }
            });
            socket.on('authenticated', function() {
              live_connection = true;
              that.setConnectionStatus('Connected');
              $('#login-form').modal('hide');
              return $('#cmd-line').focus();
            });
            socket.on('auth-error', function(e) {
              that.setConnectionStatus('Disconnected');
              $('#login-form-error').html(e);
              return $('#login-form-error').show();
            });
            socket.on('created', function() {
              live_connection = true;
              that.setConnectionStatus('Connected');
              $('#new-char-form').modal('hide');
              return $('#cmd-line').focus();
            });
            return socket.on('create-error', function(msg) {
              that.setConnectionStatus('Disconnected');
              $('#new-char-form-error').html(msg);
              return $('#new-char-form-error').show();
            });
          }]));
        };
      });
    });
    app = SecondContract.App.initInstance($(".container"));
    app.run();
    cs = SecondContract.Component.StatusDisplay.initInstance($("#connection-status"), {
      status: {
        Disconnected: "btn btn-danger",
        Connected: "btn btn-success",
        Connecting: "btn btn-warning",
        Reconnecting: "btn btn-warning"
      }
    });
    app.events.onConnectionStatusChange.addListener(cs.setStatus);
    cs.setStatus(app.getConnectionStatus());
    app.events.onLevelChange.addListener(function(l) {
      return $("#level").html(l);
    });
    app.events.onCardinalityChange.addListener(function(c) {
      return $("#cardinality").html(c);
    });
    app.events.onNameChange.addListener(function(n) {
      return $("#character-name").html(n);
    });
    app.events.onPositionChange.addListener(function(p) {
      return $("#position").text(p);
    });
    _ref = "HP SP XP Head Neck Torso LeftArm LeftLeg RightArm RightLeg LeftHand LeftFoot RightHand RightFoot".split(" ");
    _fn = function(s) {
      var pp;
      pp = SecondContract.Component.PercentProgress.initInstance($("#" + (s.toLowerCase()) + "-percent"));
      app.events["on" + s + "Change"].addListener(pp.setValue);
      return app.events["onMax" + s + "Change"].addListener(pp.setMaxValue);
    };
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      s = _ref[_i];
      _fn(s);
    }
    pp = SecondContract.Component.PercentProgress.initInstance($("#wt-percent"));
    app.events.onCarriedMassChange.addListener(pp.setValue);
    app.events.onMaxCarriedMassChange.addListener(pp.setMaxValue);
    pp = SecondContract.Component.PercentProgress.initInstance($("#cr-percent"), {
      inverted: true
    });
    app.events.onCreditUsedChange.addListener(pp.setValue);
    app.events.onCreditChange.addListener(pp.setMaxValue);
    _ref2 = "Food Drink Alcohol Caffeine Poison".split(" ");
    _fn2 = function(s) {
      pp = SecondContract.Component.PercentProgress.initInstance($("#" + (s.toLowerCase()) + "-percent"));
      return app.events["on" + s + "Change"].addListener(pp.setValue);
    };
    for (_j = 0, _len2 = _ref2.length; _j < _len2; _j++) {
      s = _ref2[_j];
      _fn2(s);
    }
    _ref3 = "Agility Charisma Coordination Durability Intelligence Luck Speed Strength Wisdom HeartRate HealRate".split(" ");
    _fn3 = function(s) {
      return app.events["on" + s + "Change"].addListener(function(v) {
        return $("#" + (s.toLowerCase()) + "-stat").html(v);
      });
    };
    for (_k = 0, _len3 = _ref3.length; _k < _len3; _k++) {
      s = _ref3[_k];
      _fn3(s);
    }
    defaultChat = MITHGrid.Presentation.initInstance($('#default-output'), {
      dataView: MITHGrid.Data.View.initInstance({
        dataStore: app.dataStore.text,
        types: ["Text"]
      }),
      lenses: {
        Text: function(container, presentation, model, id) {
          var el, item, rendering;
          item = model.getItem(id);
          el = $("<div></div>");
          el.html(item.text[0]);
          $(container).append(el);
          $(container).get(0).scrollTop = 10000000;
          rendering = {};
          rendering.remove = function() {
            return el.remove();
          };
          rendering.update = function(item) {
            return el.html(item.text[0]);
          };
          return rendering;
        }
      }
    });
    combatChat = MITHGrid.Presentation.initInstance($('#combat-output'), {
      dataView: MITHGrid.Data.View.initInstance({
        dataStore: app.dataStore.text,
        types: ["Combat"]
      }),
      lenses: {
        Combat: function(container, presentation, model, id) {
          var el, item, rendering;
          item = model.getItem(id);
          el = $("<div></div>");
          el.html(item.text[0]);
          $(container).append(el);
          $(container).get(0).scrollTop = 10000000;
          rendering = {};
          rendering.remove = function() {
            return el.remove();
          };
          rendering.update = function(item) {
            return el.html(item.text[0]);
          };
          return rendering;
        }
      }
    });
    debugChat = MITHGrid.Presentation.initInstance($('#debug-output'), {
      dataView: MITHGrid.Data.View.initInstance({
        dataStore: app.dataStore.text,
        types: ["Debug"]
      }),
      lenses: {
        Debug: function(container, presentation, model, id) {
          var el, item, rendering;
          item = model.getItem(id);
          el = $("<div></div>");
          el.html(item.text[0]);
          $(container).append(el);
          $(container).get(0).scrollTop = 10000000;
          rendering = {};
          rendering.remove = function() {
            return el.remove();
          };
          rendering.update = function(item) {
            return el.html(item.text[0]);
          };
          return rendering;
        }
      }
    });
    channelChat = MITHGrid.Presentation.initInstance($('#channels-output'), {
      dataView: MITHGrid.Data.View.initInstance({
        dataStore: app.dataStore.text,
        types: ["Emote", "Chat"]
      }),
      lenses: {
        Emote: function(container, presentation, model, id) {
          var el, item, rendering;
          item = model.getItem(id);
          el = $("<div></div>");
          el.text("(" + item.channel[0] + ") ").append(item.text[0]);
          $(container).append(el);
          $(container).get(0).scrollTop = 10000000;
          rendering = {};
          rendering.remove = function() {
            return el.remove();
          };
          rendering.update = function(item) {
            return el.html(item.text[0]);
          };
          return rendering;
        },
        Chat: function(container, presentation, model, id) {
          var el, item, rendering;
          item = model.getItem(id);
          el = $("<div></div>");
          el.text("(" + item.channel[0] + ") " + item.speaker[0] + ": ").append(item.text[0]);
          $(container).append(el);
          $(container).get(0).scrollTop = 10000000;
          rendering = {};
          rendering.remove = function() {
            return el.remove();
          };
          rendering.update = function(item) {
            return el.html(item.text[0]);
          };
          return rendering;
        }
      }
    });
    abilityInfoEl = $("<table width='100%'></table>");
    $("#ability-info").append(abilityInfoEl);
    abilities = MITHGrid.Presentation.initInstance(abilityInfoEl, {
      dataView: MITHGrid.Data.View.initInstance({
        dataStore: app.dataStore.char,
        types: ["Ability"]
      }),
      lenses: {
        Ability: function(container, presentation, model, id) {
          var el, item, mypp, nameEl, pbEl, pbpEl, progressEl, rendering;
          item = model.getItem(id);
          rendering = {};
          el = $("<tr></tr>");
          nameEl = $("<td width='50%'></td>");
          el.append(nameEl);
          pbEl = $("<div class='bar' style='width: 0%;'></div>");
          pbpEl = $("<div class='progress'></div>");
          pbpEl.append(pbEl);
          progressEl = $("<td></td>");
          el.append(progressEl);
          progressEl.append(pbpEl);
          $(container).append(el);
          mypp = SecondContract.Component.PercentProgress.initInstance(pbEl);
          rendering.update = function(item) {
            nameEl.text(item.name[0]);
            mypp.setValue(item.level[0]);
            return mypp.setMaxValue(item.maxlevel[0]);
          };
          rendering.update(item);
          rendering.remove = function() {
            if (mypp._destroy != null) mypp._destroy();
            return el.remove();
          };
          return rendering;
        }
      }
    });
    skillLens = function(minClass, maxClass) {
      if (!(maxClass != null)) maxClass = minClass;
      return function(container, presentation, model, id) {
        var el, item, mypp, myptp, nameEl, pbEl, pbpEl, pointEl, progressEl, rendering;
        item = model.getItem(id);
        if (minClass > item["class"][0] || maxClass < item["class"][0]) return;
        rendering = {};
        el = $("<tr></tr>");
        nameEl = $("<td width='50%'></td>");
        nameEl.text(item.name[0]);
        el.append(nameEl);
        pbEl = $("<div class='bar' style='width: 0%;'></div>");
        pbpEl = $("<div class='progress'></div>");
        pbpEl.append(pbEl);
        progressEl = $("<td></td>");
        el.append(progressEl);
        progressEl.append(pbpEl);
        pbpEl = $("<div class='progress'></div>");
        pointEl = $("<div class='bar bar-warning' style='width: 0%;'></div>");
        pbpEl.append(pointEl);
        progressEl.append(pbpEl);
        $(container).append(el);
        mypp = SecondContract.Component.PercentProgress.initInstance(pbEl);
        myptp = SecondContract.Component.PercentProgress.initInstance(pointEl, {
          asPercent: true
        });
        rendering.update = function(item) {
          nameEl.text(item.name[0]);
          mypp.setValue(item.level[0]);
          mypp.setMaxValue(item.maxlevel[0]);
          myptp.setValue(item.points[0]);
          return myptp.setMaxValue(item.maxpoints[0]);
        };
        rendering.update(item);
        rendering.remove = function() {
          if (mypp._destroy != null) mypp._destroy();
          return el.remove();
        };
        return rendering;
      };
    };
    primarySkills = MITHGrid.Presentation.initInstance($('#primary-skills'), {
      dataView: MITHGrid.Data.View.initInstance({
        dataStore: app.dataStore.char,
        types: ["Skill"]
      }),
      lenses: {
        Skill: skillLens(1)
      }
    });
    secondarySkills = MITHGrid.Presentation.initInstance($('#secondary-skills'), {
      dataView: MITHGrid.Data.View.initInstance({
        dataStore: app.dataStore.char,
        types: ["Skill"]
      }),
      lenses: {
        Skill: skillLens(2)
      }
    });
    otherSkills = MITHGrid.Presentation.initInstance($('#other-skills'), {
      dataView: MITHGrid.Data.View.initInstance({
        dataStore: app.dataStore.char,
        types: ["Skill"]
      }),
      lenses: {
        Skill: skillLens(3, 4)
      }
    });
    inventoryContainers = {
      book: $("<div><h6>Books</h6></div>"),
      recipe: $("<div><h6>Recipes</h6></div>"),
      clothing: $("<div><h6>Clothing</h6></div>"),
      armor: $("<div><h6>Armors</h6></div>"),
      weapon: $("<div><h6>Weapons</h6></div>"),
      material: $("<div><h6>Materials</h6></div>"),
      food: $("<div><h6>Food</h6></div>"),
      drink: $("<div><h6>Drink</h6></div>"),
      alcohol: $("<div><h6>Alcohol</h6></div>"),
      key: $("<div><h6>Keys</h6></div>"),
      misc: $("<div><h6>Miscellany</h6></div>"),
      equipped: $("<div><h6>Equipped</h6></div>")
    };
    invEl = $("#inventory-info");
    playerInventory = MITHGrid.Data.SubSet.initInstance({
      dataStore: app.dataStore.char,
      expressions: ["!env"]
    });
    playerInventory.setKey("");
    app.events.onNameChange.addListener(function(n) {
      playerInventory.setKey(n.toLowerCase());
      return playerInventory.events.onModelChange.fire(playerInventory, playerInventory.items());
    });
    lens = function(c, p, m, i) {
      var item, rEl, rendering;
      item = m.getItem(i);
      rendering = {};
      rEl = $("<div>" + item.label[0] + "</div>").appendTo(c);
      rendering.update = function(item) {
        return rEl.html(item.label[0]);
      };
      rendering.remove = function() {
        return rEl.remove();
      };
      return rendering;
    };
    _results = [];
    for (type in inventoryContainers) {
      el = inventoryContainers[type];
      _results.push((function(type, el) {
        var dv, filterFctn, innerEl, pEl, presentation;
        if (type !== "equipped" && type !== "misc") {
          filterFctn = function(model, id) {
            var item, _ref4, _ref5;
            item = model.getItem(id);
            if ((__indexOf.call(item.type, "Item") >= 0) && (__indexOf.call(item.type, type) >= 0) && !(((_ref4 = item.wielded) != null ? _ref4[0] : void 0) || ((_ref5 = item.worn) != null ? _ref5[0] : void 0))) {
              return;
            }
            return false;
          };
        } else if (type === "misc") {
          filterFctn = function(model, id) {
            var item, _ref4, _ref5;
            item = model.getItem(id);
            if ((__indexOf.call(item.type, "Item") >= 0) && (item.type.length === 1 || (__indexOf.call(item.type, "misc") >= 0)) && !(((_ref4 = item.wielded) != null ? _ref4[0] : void 0) || ((_ref5 = item.worn) != null ? _ref5[0] : void 0))) {
              return;
            }
            return false;
          };
        } else {
          filterFctn = function(model, id) {
            var item, _ref4, _ref5;
            item = model.getItem(id);
            if ((__indexOf.call(item.type, "Item") >= 0) && (((_ref4 = item.wielded) != null ? _ref4[0] : void 0) || ((_ref5 = item.worn) != null ? _ref5[0] : void 0))) {
              return;
            }
            return false;
          };
        }
        innerEl = $("<div id='item-list-" + type + "'></div>");
        el.append(innerEl);
        invEl.append(el);
        dv = MITHGrid.Data.View.initInstance({
          dataStore: playerInventory
        });
        dv.registerFilter({
          eventFilterItem: filterFctn,
          eventModelChange: function() {},
          events: {
            onFilterChange: app.events.onNameChange
          }
        });
        pEl = $("#item-list-" + type);
        presentation = MITHGrid.Presentation.initInstance(pEl, {
          dataView: dv,
          lensKey: ['"Item"'],
          lenses: {
            Item: lens
          }
        });
        return dv.events.onModelChange.addListener(function(m, i) {
          console.log(m.size());
          if (m.size() === 0) {
            return inventoryContainers[type].hide();
          } else {
            return inventoryContainers[type].show();
          }
        });
      })(type, el));
    }
    return _results;
  });

}).call(this);
