$ ->

  SC = 
    Collections: {}
    Models: {}
    Views: {}

  _.templateSettings =
    interpolate: /\{\{(.+?)\}\}/g
    variable: "data"

  later = (cb) ->
    setTimeout cb, 0

  $.ajaxSetup
    statusCode:
      401: -> window.location.replace("#login")
      403: -> window.location.replace("#denied")
  #
  # Models and Collections for various resources
  #

  SC.Models.System = Backbone.Model.extend
    url: "http://localhost:6050/api/system"

  SC.Models.system = new SC.Models.System


  SC.Models.Verb = Backbone.Model.extend
    defaults: -> {}

    urlRoot: "http://localhost:6050/api/iflib/verb"

    url: ->
      base = this.urlRoot 
      base ?= this.collection.url if this.collection?
      base ?= "/"
      if this.isNew()
        return base;

      return base + "/" + encodeURIComponent(this.id)

    initialize: ->

    validate: (attributes) ->
      if !attributes.verbs or !$.isArray(attributes.verbs) or attributes.verbs.length == 0
        return "A verb must have at least one word to identify it."

  SC.Collections.Verb = Backbone.Collection.extend
    model: SC.Models.Verb
    url: 'http://localhost:6050/api/iflib/verb'

  SC.Collections.verbs = new SC.Collections.Verb


  SC.Views.Verb = Backbone.View.extend
    initialize: ->
      this.template = _.template $('#verb-template').html()
      this.listenTo this.model, 'change', this.render

    events:
      'click .save': 'save'

    render: ->
      json = this.model.toJSON()
      this.$el.html this.template json
      if this.model.isNew()
        this.$el.find("h1").text("New Verb")
      # manage settings that are difficult to put into the templates
      if json.actor?.position?
        for pos in json.actor.position
          this.$el.find("input[type='checkbox'][name='actor.position'][value='#{pos}']").prop {checked: true}
      if json.actor?.sense?
        for pos in json.actor.sense
          this.$el.find("input[type='checkbox'][name='actor.sense'][value='#{pos}']").prop {checked: true}
      if json.actor?.act?
        for pos in json.actor.act
          this.$el.find("input[type='checkbox'][name='actor.action'][value='#{pos}']").prop {checked: true}

      for pos in ["direct", "indirect", "instrumental"]
        do (pos) =>
          posEl = this.$el.find("input[type='checkbox'][name='arguments'][value='#{pos}']")
          if json.arguments?[pos]?.requirements?
            for req in json.arguments[pos].requirements
              this.$el.find("input[type='checkbox'][name='arguments.#{pos}'][value='#{req}']").prop
                checked: true
          $(posEl).change ->
            if $(posEl).is(':checked')
              $(posEl).parent().parent().parent().find(".fieldset-body").fadeIn()
            else
              $(posEl).parent().parent().parent().find(".fieldset-body").fadeOut()

          if json.arguments?[pos]?.used
            posEl.prop
              checked: true
            later ->
              $(posEl).parent().parent().parent().find(".fieldset-body").fadeIn()
          else
            later ->
              $(posEl).parent().parent().parent().find(".fieldset-body").fadeOut()
      this.el

    save: (e) ->
      e.preventDefault()
      data =
        verbs: (this.$('input[name="verbs"]').val() + "").split(/\s*,\s*/)
        brief: this.$('input[name="brief"]').val()
        help: this.$('textarea[name="help"]').val()
        see_also: (this.$('input[name="see-also"]').val() + "").split(/\s*,\s*/)
        disabled: this.$('input[name="disabled"]').is(":checked")
        action: (this.$('textarea[name="action"]').val() + "").split(/\s*\n\s*/)
        environment:
          respiration: ($(e).val() for e in this.$('input[name="respiration"]') when $(e).is(':checked'))
          timing: ($(e).val() for e in this.$('input[name="calendar"]') when $(e).is(':checked'))
          terrain: ($(e).val() for e in this.$('input[name="terrain"]') when $(e).is(':checked'))
          weather: ($(e).val() for e in this.$('input[name="weather"]') when $(e).is(':checked'))
        arguments:
          direct:
            used: this.$('input[name="arguments"][value="direct"]').is(":checked")
            requirements: ($(e).val() for e in this.$('input[name="arguments.direct"]') when $(e).is(':checked'))
          indirect:
            used: this.$('input[name="arguments"][value="indirect"]').is(":checked")
            requirements: ($(e).val() for e in this.$('input[name="arguments.indirect"]') when $(e).is(':checked'))
          instrumental:
            used: this.$('input[name="arguments"][value="instrumental"]').is(":checked")
            requirements: ($(e).val() for e in this.$('input[name="arguments.instrumental"]') when $(e).is(':checked'))

      console.log data

      this.model.set data

      if this.model.isNew()
        SC.Collections.verbs.create this.model,
          success: => console.log "Saved!"
      else
        this.model.save()
  
      false

  SC.Views.VerbList = Backbone.View.extend
    initialize: ->
      this.model.bind 'reset', this.render, this
      this.model.bind 'add', this.appendNewVerb, this
      this.template = _.template $('#verb-list-template').html()

    render: ->
      this.$el.html this.template {} 
      this.$ul = this.$el.find("ul").first()
      for verb in this.model.models
        this.appendNewVerb verb
      this.el

    appendNewVerb: (verb) ->
      this.$ul.append(new SC.Views.VerbListItem({model:verb}).render())

  SC.Views.VerbListItem = Backbone.View.extend
    tagName: 'li'

    initialize: ->
      this.template = _.template $('#verb-list-item-template').html()
      this.model.bind 'change', this.render, this
  
    render: ->
      this.$el.html this.template this.model.toJSON()
      this.el
  
  SC.Views.System = Backbone.View.extend
    initialize: ->
      this.template = _.template $('#system-template').html()
  
    events:
      "click #save-system-name": "saveSystemInfo"
  
    saveSystemInfo: (e) ->
      e.preventDefault()
      this.model.set
        name: $("#system-name").val()
      this.model.save()
      false
  
    render: ->
      this.$el.html this.template this.model.toJSON()
      this.el
  
  SC.Views.IFLib = Backbone.View.extend
    initialize: ->
      this.template = _.template $("#iflib-template").html()
  
    render: ->
      this.$el.html this.template()
      this.el
  
  SC.Views.WorldLib = Backbone.View.extend
    initialize: ->
      this.template = _.template $("#worldlib-template").html()
  
    render: ->
      this.$el.html this.template()
      this.el
  
  SC.Views.Login = Backbone.View.extend
    initialize: ->
      this.template = _.template $("#login-template").html()
  
    render: ->
      this.$el.html this.template()
      this.el
  
  SC.Router = Backbone.Router.extend
    routes : 
      "": "system"
      "iflib/verb": "verbs"
      "iflib/verb/new": "newVerb"
      "iflib/verb/:id": "verb"
      "iflib": "iflib"
      "worldlib": "worldlib"
      "login": "login"
      "denied": "denied"
    currentViews :
      "#main": null
      "#sidebar": null

    login: ->
      this.showView "#main", new SC.Views.Login()

    denied: ->
      this.showView "#main", new SC.Views.Denied()

    system: ->
      me = this
      SC.Models.system.fetch
        success: ->
          me.showView "#main", new SC.Views.System
            model: SC.Models.system
          me.showNav ""

    iflib: ->
      this.showView "#main", new SC.Views.IFLib()
      this.showNav "iflib"

    worldlib: ->
      this.showView "#main", new SC.Views.WorldLib()
      this.showNav "worldlib"

    verbs: ->
      me = this
      SC.Collections.verbs.fetch
        success: ->
          me.showView "#sidebar", new SC.Views.VerbList
            model: SC.Collections.verbs
          me.showNav "iflib"

    newVerb: ->
      this.showView "#main", new SC.Views.Verb
        model: new SC.Collections.verbs.model
      this.showNav "iflib"

    verb: (id) ->
      this.showView "#main", new SC.Views.Verb
        model: SC.Collections.verbs.get id
      this.showNav "iflib"

    showView: (selector, view) ->
      if this.currentViews[selector]?.close?
        this.currentViews[selector].close()
      $(selector).html view.render()
      this.currentViews[selector] = view
      view

    showNav: (sel) ->
      if this.currentNav? and this.currentNav != sel
        if this.currentViews["#sidebar"]?.close?
          this.currentViews["#sidebar"].close()
        $("#sidebar").empty()
        delete this.currentViews["#sidebar"]
      this.currentNav = sel
      for el in $(".navbar ul.nav li")
        if el
          if $(el).find("a").attr("href") == "#"+sel
            $(el).addClass "active"
          else
            $(el).removeClass "active"

    before: (callback) ->
      if callback?
        callback.call this
  
  SC.Models.system.fetch
    success: ->
      SC.App = new SC.Router

      $("a.brand").text(SC.Models.system.attributes.name)

      SC.Models.system.on "change:name", (model, name) ->
        $("a.brand").text(name)

      Backbone.history.start()
