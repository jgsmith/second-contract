$ ->

  SC = 
    Collections: {}
    Models: {}
    Views: {}

  _.templateSettings =
    interpolate: /\{\{(.+?)\}\}/g


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

    render: ->
      this.$el.html this.template(this.model.toJSON())
      this.el;

    events:
      'click .save': 'save'

    save: ->
      this.model.set
        verbs: $('#verb-verbs').split(/\s*,\s*/)
        help: $('#verb-help')

      if this.model.isNew()
        self = this;
        SC.Collections.verbs.create this.model,
          success: ->
      else
        this.model.save()
  
      false

  SC.Views.VerbList = Backbone.View.extend
    tagName: "ul"

    initialize: ->
      this.model.bind 'reset', this.render, this
      this.model.bind 'add', this.appendNewVerb, this

    render: ->
      for verb in this.model.models
        this.appendNewVerb verb
      this.el

    appendNewVerb: (verb) ->
      this.$el.append(new SC.Views.VerbListItem({model:verb}).render())

  SC.Views.VerbListItem = Backbone.View.extend
    tagName: 'li'

    initialize: ->
      this.template = _.template $("#verb-list-item-template").html()
      this.model.bind 'change', this.render, this
  
    render: ->
      this.$el.html this.template this.model.toJSON()
      this.el
  
  SC.Views.Header = Backbone.View.extend
    initialize: ->
      this.template = _.template $("#header-template").html()
  
    render: ->
      this.$el.html this.template()
      this.el
  
    events: { }
    
  SC.Views.System = Backbone.View.extend
    initialize: ->
      this.template = _.template $("#system-template").html()
  
    events:
      "click #save-system-name": "saveSystemInfo"
  
    saveSystemInfo: ->
      this.model.set
        name: $("#system-name").val()
      this.model.save()
  
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
      "iflib/verb/:id": "verb"
      "iflib": "iflib"
      "worldlib": "worldlib"
      "login": "login"
      "denied": "denied"

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
          me.showView "#main", new SC.Views.VerbList
            model: SC.Collections.verbs
          me.showNav "iflib"

    verb: (id) ->
      this.showView "#main", new SC.Views.Verb
        model: SC.Collections.verbs.get id
      this.showNav "iflib"

    showView: (selector, view) ->
      if this.currentView?.close?
        this.currentView.close()
      $(selector).html view.render()
      this.currentView = view
      view

    showNav: (sel) ->
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
