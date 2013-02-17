$ ->
  $('#login-form').modal
    backdrop: 'static'
    keyboard: false
    show: false

  $('#login-form').on 'show',  -> $('#login-form-error').hide()
  $('#login-form').on 'shown', -> $('#inputUsername').focus()

  $('#login-submit').click ->
    $('#login-form-error').hide()
    app.emit 'authenticate', $('#inputUsername').val(), $('#inputPassword').val()
    false

  $('#new-char-form').modal
    keyboard: true
    show: false

  $('#new-char-form').on 'show', ->
    $('#new-char-form-error').hide()

  $('#new-char-form').on 'shown', ->
    $('#newUsername').focus()

  $('#new-character').click ->
    $('#login-form').modal('hide')
    $('#new-char-form').modal('show')
    false

  $('#new-char-cancel').click ->
    $('#new-char-form').modal('hide')
    $('#login-form').modal('show')
    false

  $('#new-char-submit').click ->
    $('#new-char-form-error').hide()
    app.emit 'create',
      name: $('#newUsername').val()
      capname: $('#newCapName').val()
      password: $('#newPassword').val()
      password2: $('#newPassword2').val()
      gender: $('#newGender').val()
      realname: $('#newRealName').val()
      ofage: $('#newAgeCheck').val()
      email: $('#newEmail').val()
    false

  $('#news-display-close').click ->
    $('#news-display').modal('hide')
    false

  $('#news-display').modal
    show: false

  $('#page-display-close').click ->
    $('#page-display').modal('hide')
    false

  $('#page-display').modal
    show: false

  $('#send-command').submit ->
    cmd = $('#cmd-line').val()
    app.emit 'parse', $('#cmd-line').val()
    
    $('#cmd-line').focus()
    $('#cmd-line').select()
    false

  MITHGrid.defaults 'SecondContract.Component.StatusDisplay',
    variables:
      Status:
        is: 'rw'

  MITHGrid.defaults 'SecondContract.Component.SimpleStatistic',
    variables:
      Value:
        is: 'rw'
      MaxValue:
        is: 'rw'

  MITHGrid.defaults 'SecondContract.Component.PercentProgress',
    variables:
      Value:
        is: 'rw'
      MaxValue:
        is: 'rw'
        default: 100

  MITHGrid.defaults 'SecondContract.App',
    dataStores:
      char: {}
      text: {}
      inventory: {}
    variables:
      ConnectionStatus: { is: 'rw' }
      Name:        { is: 'rw' }
      Level:       { is: 'rw' }
      Position:    { is: 'rw' }
      Cardinality: { is: 'rw' }
      Credit:     { is: 'rw' }
      CreditUsed: { is: 'rw' }
      Environment:    { is: 'rw' }
      CarriedMass:    { is: 'rw' }
      MaxCarriedMass: { is: 'rw' }
      HP:    { is: 'rw' }
      MaxHP: { is: 'rw' }
      SP:    { is: 'rw' }
      MaxSP: { is: 'rw' }
      XP:    { is: 'rw' }
      MaxXP: { is: 'rw' }
      Food:     { is: 'rw' }
      Drink:    { is: 'rw' }
      Alcohol:  { is: 'rw' }
      Caffeine: { is: 'rw' }
      Poison:   { is: 'rw' }
      Agility:      { is: 'rw' }
      Charisma:     { is: 'rw' }
      Coordination: { is: 'rw' }
      Durability:   { is: 'rw' }
      Intelligence: { is: 'rw' }
      Luck:         { is: 'rw' }
      Speed:        { is: 'rw' }
      Strength:     { is: 'rw' }
      Wisdom:       { is: 'rw' }
      HeartRate:    { is: 'rw' }
      HealRate:     { is: 'rw' }
      Head:         { is: 'rw' }
      MaxHead:      { is: 'rw' }
      Neck:         { is: 'rw' }
      MaxNeck:      { is: 'rw' }
      Torso:        { is: 'rw' }
      MaxTorso:     { is: 'rw' }
      LeftArm:      { is: 'rw' }
      MaxLeftArm:   { is: 'rw' }
      LeftLeg:      { is: 'rw' }
      MaxLeftLeg:   { is: 'rw' }
      RightArm:     { is: 'rw' }
      MaxRightArm:  { is: 'rw' }
      RightLeg:     { is: 'rw' }
      MaxRightLeg:  { is: 'rw' }
      LeftHand:     { is: 'rw' }
      MaxLeftHand:  { is: 'rw' }
      RightHand:    { is: 'rw' }
      MaxRightHand: { is: 'rw' }
      LeftFoot:     { is: 'rw' }
      MaxLeftFoot:  { is: 'rw' }
      RightFoot:    { is: 'rw' }
      MaxRightFoot: { is: 'rw' }

  MITHGrid.globalNamespace 'SecondContract', (SecondContract) ->
    SecondContract.namespace 'Component', (Component) ->
      Component.namespace 'StatusDisplay', (CS) ->
        CS.initInstance = (args...) ->
          MITHGrid.initInstance 'SecondContract.Component.StatusDisplay', args..., (that, container) ->
            options = that.options
            that.onDestroy that.events.onStatusChange.addListener (s) ->
              $(container).attr "class", ""
              $(container).text ''
              if options.status[s]?
                $(container).addClass options.status[s]
                $(container).text s

      Component.namespace 'SimpleStatistic', (SS) ->
        SS.initInstance = (args...) ->
          MITHGrid.initInstance 'SecondContract.Component.SimpleStatistic', args..., (that, container) ->
            options = that.options
            that.onDestroy that.events.onValueChange.addListener (v) ->
              if v > that.getMaxValue()
                v = that.getMaxValue()
              $(container).text(v)
            that.onDestroy that.events.onMaxValueChange.addListener (v) ->
              if v > that.getValue()
                v = that.getValue()
              $(container).text(v)

      Component.namespace 'PercentProgress', (PP) ->
        PP.initInstance = (args...) ->
          MITHGrid.initInstance 'SecondContract.Component.PercentProgress', args..., (that, container) ->
            options = that.options

            drawBar = ->
              v = parseInt(that.getValue(),10)
              m = parseInt(that.getMaxValue(),10)
              m = 1 if m == 0
              v = m if v > m
              v = parseInt(100 * v / m, 10)
              v = 100 - v if options.inverted

              $(container).css("width", v + "%")
              if options.asPercent
                $(container).attr("title", v + " / 100")
              else
                if options.inverted
                  $(container).attr("title", (that.getMaxValue() - that.getValue()) + " / " + that.getMaxValue())
                else
                  $(container).attr("title", that.getValue() + " / " + that.getMaxValue())
              
            that.onDestroy that.events.onValueChange.addListener drawBar
            that.onDestroy that.events.onMaxValueChange.addListener drawBar

    SecondContract.namespace 'App', (App) ->
      App.initInstance = (args...) ->
        MITHGrid.Application.initInstance 'SecondContract.App', args..., (that, container) ->
          live_connection = false
          socket = io.connect()
          authenticated = false
          options = that.options

          that.emit = (args...) -> socket.emit(args...)

          checkFocus = ->
            if authenticated
              $('#cmd-line').focus()
            setTimeout checkFocus, 1
          checkFocus()

          socket.on 'connect',      -> that.setConnectionStatus "Connected"
          socket.on 'reconnect',    -> that.setConnectionStatus "Connected"
          socket.on 'reconnecting', -> that.setConnectionStatus "Reconnecting"

          socket.on 'Quit', ->
            that.setConnectionStatus "Disconnected"
            $('#inputUsername').val("")
            $('#inputPassword').val("")
            authenticated = false
            live_connection = false
             

          socket.on 'level', that.setLevel
          socket.on 'class', that.setCardinality
          socket.on 'cname', that.setName
          socket.on 'hp',    that.setHP
          socket.on 'maxhp', that.setMaxHP
          socket.on 'sp',    that.setSP
          socket.on 'maxsp', that.setMaxSP
          socket.on 'xp',    that.setXP
          socket.on 'maxxp', that.setMaxXP
          socket.on 'carried-mass',     (x) -> that.setCarriedMass    x/20
          socket.on 'max-carried-mass', (x) -> that.setMaxCarriedMass x/20
          socket.on 'credit',     that.setCredit
          socket.on 'creditUsed', that.setCreditUsed
          socket.on 'position',   that.setPosition
          socket.on 'food',    that.setFood
          socket.on 'drink',   that.setDrink
          socket.on 'alcohol', that.setAlcohol
          socket.on 'caffein', that.setCaffeine
          socket.on 'poison',  that.setPoison
          socket.on 'agility',      that.setAgility
          socket.on 'charisma',     that.setCharisma
          socket.on 'coordination', that.setCoordination
          socket.on 'durability',   that.setDurability
          socket.on 'intelligence', that.setIntelligence
          socket.on 'luck',         that.setLuck
          socket.on 'speed',        that.setSpeed
          socket.on 'strength',     that.setStrength
          socket.on 'wisdom',       that.setWisdom
          socket.on 'heartrate',    that.setHeartRate
          socket.on 'healing',      that.setHealRate
          socket.on 'head', (v) -> 
            that.setHead v.hp
            that.setMaxHead v.maxhp
          socket.on 'neck', (v) -> 
            that.setNeck v.hp
            that.setMaxNeck v.maxhp
          socket.on 'torso', (v) -> 
            that.setTorso v.hp
            that.setMaxTorso v.maxhp
          socket.on 'left-arm', (v) -> 
            that.setLeftArm v.hp
            that.setMaxLeftArm v.maxhp
          socket.on 'left-leg', (v) -> 
            that.setLeftLeg v.hp
            that.setMaxLeftLeg v.maxhp
          socket.on 'right-arm', (v) -> 
            that.setRightArm v.hp
            that.setMaxRightArm v.maxhp
          socket.on 'right-leg', (v) -> 
            that.setRightLeg v.hp
            that.setMaxRightLeg v.maxhp
          socket.on 'left-hand', (v) -> 
            that.setLeftHand v.hp
            that.setMaxLeftHand v.maxhp
          socket.on 'left-foot', (v) -> 
            that.setLeftFoot v.hp
            that.setMaxLeftFoot v.maxhp
          socket.on 'right-hand', (v) -> 
            that.setRightHand v.hp
            that.setMaxRightHand v.maxhp
          socket.on 'right-foot', (v) -> 
            that.setRightFoot v.hp
            that.setMaxRightFoot v.maxhp

          # items are identified by their clone number
          # their type is based on their type for the inventory command
          socket.on 'item', (msg) ->
            if msg.env == "FURNACE"
              that.dataStore.char.removeItems [ msg.id ]
            else
              item = that.dataStore.char.getItem msg.id
              if item.type?
                f = that.dataStore.char.updateItems
              else
                f = that.dataStore.char.loadItems
              i =
                id: msg.id
                label: msg.label
                name: msg.keyame
                type: if msg.type? then [msg.type, "Item"] else [ "Item" ]
                wielded: msg.wielded == "1"
                container: msg.container == "1"
                worn: msg.worn == "1"
                env: msg.env
              f [ i ]

          socket.on 'ability', (msg) ->
           item = that.dataStore.char.getItem "ability-#{msg.name}"
           if item.type?
              f = that.dataStore.char.updateItems
            else
              f = that.dataStore.char.loadItems
            f [
              id: "ability-#{msg.name}"
              type: "Ability"
              name: msg.name
              level: parseInt(msg.level, 10)
              maxlevel: parseInt(msg.maxlevel, 10)
            ]

          socket.on 'skill', (msg) ->
            item = that.dataStore.char.getItem "skill-#{msg.name}"
            if item.type?
              f = that.dataStore.char.updateItems
            else
              f = that.dataStore.char.loadItems
            f [
              id: "skill-#{msg.name}"
              name: msg.name
              type: "Skill"
              level: parseInt(msg.level,10)
              maxlevel: parseInt(msg.maxlevel,10)
              "class": parseInt(msg["class"],10)
              points: parseInt(msg.points,10)
              maxpoints: parseInt(msg.maxpoints,10)
            ]
            
          combatId = 0
          socket.on 'combat', (m) -> 
            that.dataStore.text.loadItems [
              id: "combat-#{combatId}"
              type: "Combat"
              text: m
            ]
            combatId += 1

          textId = 0
          socket.on 'text', (m) ->
            that.dataStore.text.loadItems [
              id: "text-#{textId}"
              type: "Debug"
              text: m
            ]
            textId += 1

          envId = 0
          socket.on 'env', (m) ->
            that.dataStore.text.loadItems [
              id: "env-#{envId}"
              type: "Text"
              text: m
            ]
            envId += 1

          chatId = 0
          socket.on 'chat', (m) ->
            that.dataStore.text.loadItems [
              id: "chat-#{chatId}"
              type: "Chat"
              channel: m.channel
              speaker: m.speaker
              text: m.text
            ]
            chatId += 1
          socket.on 'emote', (m) ->
            that.dataStore.text.loadItems [
              id: "emote-#{chatId}"
              type: "Emote"
              channel: m.channel
              speaker: m.speaker
              text: m.text
            ]
            chatId += 1


          $('#outputTabs a:first').tab 'show'

          $('#charTabs a:first').tab 'show'

          socket.on 'news', (n) ->
            $('#news-display-body').html(n)
            $('#news-display').modal('show')

          socket.on 'page', (lines) ->
            title = lines.shift()
            $('#page-title').html(title)
            $('#page-display-body').html(lines.join("<br>"))
            $('#page-display').modal('show')

          socket.on 'room', (msg) ->
            if msg.id?
              that.setEnvironment msg.id
            $('#room-name').html(msg.name || "")
            $('#room-description').html(msg.description || "")
            if msg.minimap?
              $('#room-minimap').html(msg.minimap)
              $('#room-minimap').show()
            else 
              $('#room-minimap').hide()
            $('#room-weather').html(msg.weather || "")
            $('#room-items').html(msg.items || "")
            $('#room-npcs').html(msg.npcs || "")
            $('#room-senses').html(
              (msg.sound || "") + " " + (msg.smell || "") + " " + (msg.touch || "")
            )
            html = ""
            if msg.exit?
              for k, einfo of msg.exit
                if msg.exit.hasOwnProperty(k)
                  html += "&nbsp;&nbsp;&nbsp;" + einfo.name + " [" + einfo.title + "]<br/>";
              if html != ""
                html = "Obvious exits:<br/>" + html
            $('#room-exits').html(html)

          socket.on 'authenticate', (e) ->
            that.setConnectionStatus 'Connecting'
            if $('#inputUsername').val() != "" && $('#inputPassword').val() != ""
              socket.emit 'authenticate', $('#inputUsername').val(), $('#inputPassword').val()
            else
              $('#login-form').modal('show')
              $('#inputUsername').focus()

          socket.on 'authenticated', ->
            live_connection = true
            that.setConnectionStatus 'Connected'
            $('#login-form').modal('hide')
            $('#cmd-line').focus()

          socket.on 'auth-error', (e) ->
            that.setConnectionStatus 'Disconnected'
            $('#login-form-error').html(e)
            $('#login-form-error').show()

          socket.on 'created', ->
            live_connection = true
            that.setConnectionStatus 'Connected'
            $('#new-char-form').modal('hide')
            $('#cmd-line').focus()

          socket.on 'create-error', (msg) ->
            that.setConnectionStatus 'Disconnected'
            $('#new-char-form-error').html(msg)
            $('#new-char-form-error').show()

  app = SecondContract.App.initInstance($(".container"))
  app.run()

  cs = SecondContract.Component.StatusDisplay.initInstance $("#connection-status"),
    status:
      Disconnected: "btn btn-danger"
      Connected: "btn btn-success"
      Connecting: "btn btn-warning"
      Reconnecting: "btn btn-warning"
  app.events.onConnectionStatusChange.addListener cs.setStatus
  cs.setStatus app.getConnectionStatus()

  app.events.onLevelChange.addListener (l) -> $("#level").html(l)
  app.events.onCardinalityChange.addListener (c) -> $("#cardinality").html(c)
  app.events.onNameChange.addListener (n) -> $("#character-name").html(n)
  app.events.onPositionChange.addListener (p) -> $("#position").text(p)

  for s in "HP SP XP Head Neck Torso LeftArm LeftLeg RightArm RightLeg LeftHand LeftFoot RightHand RightFoot".split(" ")
    do (s) ->
      pp = SecondContract.Component.PercentProgress.initInstance $("##{s.toLowerCase()}-percent")
      app.events["on#{s}Change"].addListener pp.setValue
      app.events["onMax#{s}Change"].addListener pp.setMaxValue

  pp = SecondContract.Component.PercentProgress.initInstance $("#wt-percent")
  app.events.onCarriedMassChange.addListener pp.setValue
  app.events.onMaxCarriedMassChange.addListener pp.setMaxValue

  pp = SecondContract.Component.PercentProgress.initInstance $("#cr-percent"),
    inverted: true

  app.events.onCreditUsedChange.addListener pp.setValue
  app.events.onCreditChange.addListener pp.setMaxValue

  for s in "Food Drink Alcohol Caffeine Poison".split(" ")
    do (s) ->
      pp = SecondContract.Component.PercentProgress.initInstance $("##{s.toLowerCase()}-percent")
      app.events["on#{s}Change"].addListener pp.setValue
  

  # stats are simple statistics

  for s in "Agility Charisma Coordination Durability Intelligence Luck Speed Strength Wisdom HeartRate HealRate".split(" ")
    do (s) ->
      app.events["on#{s}Change"].addListener (v) ->
        $("##{s.toLowerCase()}-stat").html(v)

  #
  # TODO: add badges for number of unread messages/lines (non-combat)
  #       for combat/default, indicate that there are lines since last active
  #
  defaultChat = MITHGrid.Presentation.initInstance $('#default-output'),
    dataView: MITHGrid.Data.View.initInstance
      dataStore: app.dataStore.text
      types: [ "Text" ]
    lenses:
      Text: (container, presentation, model, id) ->
        item = model.getItem id
        el = $("<div></div>")
        el.html(item.text[0])
        $(container).append(el)
        $(container).get(0).scrollTop = 10000000;
        rendering = {}
        rendering.remove = -> el.remove()
        rendering.update = (item) -> el.html(item.text[0])
        rendering

  combatChat = MITHGrid.Presentation.initInstance $('#combat-output'),
    dataView: MITHGrid.Data.View.initInstance
      dataStore: app.dataStore.text
      types: [ "Combat" ]
    lenses:
      Combat: (container, presentation, model, id) ->
        item = model.getItem id
        el = $("<div></div>")
        el.html(item.text[0])
        $(container).append(el)
        $(container).get(0).scrollTop = 10000000;
        rendering = {}
        rendering.remove = -> el.remove()
        rendering.update = (item) -> el.html(item.text[0])
        rendering

  debugChat = MITHGrid.Presentation.initInstance $('#debug-output'),
    dataView: MITHGrid.Data.View.initInstance
      dataStore: app.dataStore.text
      types: [ "Debug" ]
    lenses:
      Debug: (container, presentation, model, id) ->
        item = model.getItem id
        el = $("<div></div>")
        el.html(item.text[0])
        $(container).append(el)
        $(container).get(0).scrollTop = 10000000;
        rendering = {}
        rendering.remove = -> el.remove()
        rendering.update = (item) -> el.html(item.text[0])
        rendering

  channelChat = MITHGrid.Presentation.initInstance $('#channels-output'),
    dataView: MITHGrid.Data.View.initInstance
      dataStore: app.dataStore.text
      types: [ "Emote", "Chat" ]
    lenses:
      Emote: (container, presentation, model, id) ->
        item = model.getItem id
        el = $("<div></div>")
        el.text("(" + item.channel[0] + ") ").append(item.text[0])
        $(container).append(el)
        $(container).get(0).scrollTop = 10000000;
        rendering = {}
        rendering.remove = -> el.remove()
        rendering.update = (item) -> el.html(item.text[0])
        rendering

      Chat: (container, presentation, model, id) ->
        item = model.getItem id
        el = $("<div></div>")
        el.text("(" + item.channel[0] + ") " + item.speaker[0] + ": ").append(item.text[0])
        $(container).append(el)
        $(container).get(0).scrollTop = 10000000;
        rendering = {}
        rendering.remove = -> el.remove()
        rendering.update = (item) -> el.html(item.text[0])
        rendering

  abilityInfoEl = $("<table width='100%'></table>")
  $("#ability-info").append(abilityInfoEl)
  abilities = MITHGrid.Presentation.initInstance abilityInfoEl,
    dataView: MITHGrid.Data.View.initInstance
      dataStore: app.dataStore.char
      types: [ "Ability" ]
    lenses:
      Ability: (container, presentation, model, id) ->
        item = model.getItem id
        rendering = {}
        el = $("<tr></tr>")
        nameEl = $("<td width='50%'></td>")
        el.append(nameEl)

        pbEl = $("<div class='bar' style='width: 0%;'></div>")
        pbpEl = $("<div class='progress'></div>")
        pbpEl.append(pbEl)
        progressEl = $("<td></td>")
        el.append(progressEl)
        progressEl.append(pbpEl)

        $(container).append(el)

        mypp = SecondContract.Component.PercentProgress.initInstance pbEl

        rendering.update = (item) ->
          nameEl.text(item.name[0])
          mypp.setValue item.level[0]
          mypp.setMaxValue item.maxlevel[0]

        rendering.update(item)
        rendering.remove = ->
          mypp._destroy() if mypp._destroy?
          el.remove()
        rendering
        
  # now we need to do the skill listings
  # we want three presentations:
  # type: Skill; .class = 1
  # type: Skill; .class = 2
  # type: Skill; .class != [1, 2]
  
  skillLens = (minClass, maxClass) ->
    maxClass = minClass if !maxClass?
    (container, presentation, model, id) ->
      item = model.getItem id
      return if minClass > item["class"][0] or maxClass < item["class"][0]
      rendering = {}
      el = $("<tr></tr>")
      nameEl = $("<td width='50%'></td>")
      nameEl.text(item.name[0])
      el.append(nameEl)
      pbEl = $("<div class='bar' style='width: 0%;'></div>")
      pbpEl = $("<div class='progress'></div>")
      pbpEl.append(pbEl)
      progressEl = $("<td></td>")
      el.append(progressEl)
      progressEl.append(pbpEl)

      pbpEl = $("<div class='progress'></div>");
      pointEl = $("<div class='bar bar-warning' style='width: 0%;'></div>")
      pbpEl.append(pointEl)
      progressEl.append(pbpEl)

      $(container).append(el)

      mypp = SecondContract.Component.PercentProgress.initInstance pbEl
      myptp = SecondContract.Component.PercentProgress.initInstance pointEl,
        asPercent: true
      
      rendering.update = (item) ->
        nameEl.text(item.name[0])
        mypp.setValue item.level[0]
        mypp.setMaxValue item.maxlevel[0]
        myptp.setValue item.points[0]
        myptp.setMaxValue item.maxpoints[0]
      rendering.update(item)
      rendering.remove = ->
        mypp._destroy() if mypp._destroy?
        el.remove()
      rendering

  primarySkills = MITHGrid.Presentation.initInstance $('#primary-skills'),
    dataView: MITHGrid.Data.View.initInstance
      dataStore: app.dataStore.char
      types: [ "Skill" ]
    lenses:
      Skill: skillLens 1 
  secondarySkills = MITHGrid.Presentation.initInstance $('#secondary-skills'),
    dataView: MITHGrid.Data.View.initInstance
      dataStore: app.dataStore.char
      types: [ "Skill" ]
    lenses:
      Skill: skillLens 2 
  otherSkills = MITHGrid.Presentation.initInstance $('#other-skills'),
    dataView: MITHGrid.Data.View.initInstance
      dataStore: app.dataStore.char
      types: [ "Skill" ]
    lenses:
      Skill: skillLens 3,4

  # inventory... we want a section for each type of item
  # we'll only show primary inventory -- that is, items that are directly
  # in the player's inventory - not things that are in things in ...
  inventoryContainers =
    book: $("<div><h6>Books</h6></div>")
    recipe: $("<div><h6>Recipes</h6></div>")
    clothing: $("<div><h6>Clothing</h6></div>")
    armor: $("<div><h6>Armors</h6></div>")
    weapon: $("<div><h6>Weapons</h6></div>")
    material: $("<div><h6>Materials</h6></div>")
    food: $("<div><h6>Food</h6></div>")
    drink: $("<div><h6>Drink</h6></div>")
    alcohol: $("<div><h6>Alcohol</h6></div>")
    key: $("<div><h6>Keys</h6></div>")
    misc: $("<div><h6>Miscellany</h6></div>")
    equipped: $("<div><h6>Equipped</h6></div>")

  invEl = $("#inventory-info")
  playerInventory = MITHGrid.Data.SubSet.initInstance
    dataStore: app.dataStore.char
    expressions: [ "!env" ]
  playerInventory.setKey ""
  app.events.onNameChange.addListener (n) ->
    playerInventory.setKey n.toLowerCase()
    playerInventory.events.onModelChange.fire playerInventory, playerInventory.items()

  lens = (c, p, m, i) ->
    item = m.getItem i
    rendering = {}
    rEl = $("<div>" + item.label[0] + "</div>").appendTo(c)
    rendering.update = (item) ->
      rEl.html(item.label[0])
    rendering.remove = -> 
      rEl.remove()
    rendering
  for type, el of inventoryContainers
    do (type, el) ->
      if type not in [ "equipped", "misc" ]
        filterFctn = (model, id) ->
          item = model.getItem id
          if ("Item" in item.type) and (type in item.type) and !(item.wielded?[0] || item.worn?[0])
            return
          return false
      else if type == "misc"
        filterFctn = (model, id) ->
          item = model.getItem id
          if ("Item" in item.type) and (item.type.length == 1 or ("misc" in item.type)) and !(item.wielded?[0] || item.worn?[0])
            return
          return false
      else
        filterFctn = (model, id) ->
          item = model.getItem id
          if ("Item" in item.type) and (item.wielded?[0] or item.worn?[0])
            return
          return false
      innerEl = $("<div id='item-list-#{type}'></div>")
      el.append(innerEl)
      invEl.append(el)
      dv = MITHGrid.Data.View.initInstance
        dataStore: playerInventory
      dv.registerFilter
        eventFilterItem: filterFctn
        eventModelChange: ->
        events:
          onFilterChange: app.events.onNameChange
      
      pEl = $("#item-list-#{type}")
      presentation = MITHGrid.Presentation.initInstance pEl,
        dataView: dv
        lensKey: [ '"Item"' ]
        lenses:
          Item: lens
      dv.events.onModelChange.addListener (m,i) ->
        console.log m.size()
        if m.size() == 0
          inventoryContainers[type].hide()
        else
          inventoryContainers[type].show()
