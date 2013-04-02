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
